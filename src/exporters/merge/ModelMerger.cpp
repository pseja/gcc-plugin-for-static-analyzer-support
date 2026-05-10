/**
 * @file ModelMerger.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements merging of multiple translation-unit models into one whole-program model.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ModelMerger.hpp"
#include "IdMaps.hpp"

#include "ArrayType.hpp"
#include "Block.hpp"
#include "ComplexType.hpp"
#include "FieldVariable.hpp"
#include "Function.hpp"
#include "FunctionType.hpp"
#include "Instruction.hpp"
#include "Linkage.hpp"
#include "Operand.hpp"
#include "PhiIncomingValue.hpp"
#include "PointerType.hpp"
#include "StandardVariable.hpp"
#include "StructType.hpp"
#include "SwitchCase.hpp"
#include "Type.hpp"
#include "TypeData.hpp"
#include "TypeKind.hpp"
#include "UnionType.hpp"
#include "Variable.hpp"

#include "AssignInstruction.hpp"
#include "CallInstruction.hpp"
#include "CondInstruction.hpp"
#include "ClobberInstruction.hpp"
#include "GotoInstruction.hpp"
#include "LabelInstruction.hpp"
#include "AsmInstruction.hpp"
#include "PhiInstruction.hpp"
#include "ReturnInstruction.hpp"
#include "SwitchInstruction.hpp"

#include "utility.hpp"

namespace CodeListener::Exporters
{

/**
 * Map a source TypeId to its merged-model equivalent using the provided id maps.
 *
 * @param id   Source TypeId to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Corresponding TypeId in the merged model.
 */
static Core::TypeId remapType(Core::TypeId id, const IdMaps &maps)
{
    if (!id.isValid())
    {
        return id;
    }

    auto it = maps.type_map.find(id);
    assert(it != maps.type_map.end() && "type ID not found in IdMaps");
    return it->second;
}

/**
 * Map a source VariableId to its merged-model equivalent.
 *
 * @param id   Source VariableId to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Corresponding VariableId in the merged model.
 */
static Core::VariableId remapVar(Core::VariableId id, const IdMaps &maps)
{
    if (!id.isValid())
    {
        return id;
    }

    auto it = maps.var_map.find(id);
    assert(it != maps.var_map.end() && "variable ID not found in IdMaps");
    return it->second;
}

/**
 * Map a source FunctionId to its merged-model equivalent.
 *
 * @param id   Source FunctionId to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Corresponding FunctionId in the merged model.
 */
static Core::FunctionId remapFunc(Core::FunctionId id, const IdMaps &maps)
{
    if (!id.isValid())
    {
        return id;
    }

    auto it = maps.func_map.find(id);
    assert(it != maps.func_map.end() && "function ID not found in IdMaps");
    return it->second;
}

/**
 * Map a source BlockId to its merged-model equivalent.
 *
 * @param id   Source BlockId to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Corresponding BlockId in the merged model.
 */
static Core::BlockId remapBlock(Core::BlockId id, const IdMaps &maps)
{
    if (!id.isValid())
    {
        return id;
    }

    auto it = maps.block_map.find(id);
    assert(it != maps.block_map.end() && "block ID not found in IdMaps");
    return it->second;
}

/**
 * Map a source InstructionId to its merged-model equivalent.
 *
 * @param id   Source InstructionId to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Corresponding InstructionId in the merged model.
 */
static Core::InstructionId remapInstr(Core::InstructionId id, const IdMaps &maps)
{
    if (!id.isValid())
    {
        return id;
    }

    auto it = maps.instr_map.find(id);
    assert(it != maps.instr_map.end() && "instruction ID not found in IdMaps");
    return it->second;
}

// forward declarations
static Core::Operand remapOperand(const Core::Operand &op, const IdMaps &maps);
static Core::Initializer remapInitializer(const Core::Initializer &init, const IdMaps &maps);

/**
 * Deep-copy and remap all id references inside an Accessor.
 *
 * @param acc  Accessor to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped Accessor with all ids pointing into the merged model.
 */
static Core::Accessor remapAccessor(const Core::Accessor &acc, const IdMaps &maps)
{
    Core::Accessor result;
    result.kind = acc.kind;
    result.data = std::visit(overloaded{[&](const Core::DerefAccessor &d) -> Core::AccessorData { return d; },
                                        [&](const Core::ArrayAccessor &a) -> Core::AccessorData {
                                            return Core::ArrayAccessor{remapOperand(a.index, maps)};
                                        },
                                        [&](const Core::FieldAccessor &f) -> Core::AccessorData {
                                            return Core::FieldAccessor{remapVar(f.field_id, maps)};
                                        },
                                        [&](const Core::AddressOfAccessor &a) -> Core::AccessorData {
                                            return Core::AddressOfAccessor{remapType(a.target_type_id, maps)};
                                        },
                                        [&](const Core::OffsetAccessor &o) -> Core::AccessorData {
                                            return Core::OffsetAccessor{remapOperand(o.offset, maps)};
                                        },
                                        [&](const Core::BitSliceAccessor &b) -> Core::AccessorData { return b; }},
                             acc.data);
    return result;
}

/**
 * Deep-copy and remap all id references inside an Operand.
 *
 * @param op   Operand to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped Operand with all ids pointing into the merged model.
 */
static Core::Operand remapOperand(const Core::Operand &op, const IdMaps &maps)
{
    return std::visit(overloaded{[&](const Core::ConstantOperand &co) -> Core::Operand {
                                     return Core::ConstantOperand{remapType(co.type_id, maps), co.value};
                                 },
                                 [&](const Core::VariableOperand &vo) -> Core::Operand {
                                     Core::VariableOperand result;
                                     result.id = remapVar(vo.id, maps);
                                     for (const auto &acc : vo.access_path)
                                     {
                                         result.access_path.push_back(remapAccessor(acc, maps));
                                     }
                                     if (vo.result_type_id)
                                     {
                                         result.result_type_id = remapType(*vo.result_type_id, maps);
                                     }
                                     return result;
                                 }},
                      op);
}

/**
 * Deep-copy and remap all id references inside an Initializer.
 *
 * @param init Initializer to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped Initializer with all ids pointing into the merged model.
 */
static Core::Initializer remapInitializer(const Core::Initializer &init, const IdMaps &maps)
{
    return std::visit(overloaded{[&](const Core::Operand &op) -> Core::Initializer { return remapOperand(op, maps); },
                                 [&](const std::shared_ptr<Core::InitializerList> &list) -> Core::Initializer {
                                     auto result = std::make_shared<Core::InitializerList>();
                                     if (!list)
                                     {
                                         return result;
                                     }

                                     for (const auto &elem : list->elements)
                                     {
                                         result->elements.push_back(remapInitializer(elem, maps));
                                     }

                                     return result;
                                 }},
                      init);
}

/**
 * Deep-copy and remap all id references inside a SwitchCase.
 *
 * @param sc   SwitchCase to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped SwitchCase with all ids pointing into the merged model.
 */
static Core::SwitchCase remapSwitchCase(const Core::SwitchCase &sc, const IdMaps &maps)
{
    Core::SwitchCase result;
    result.target_block_id = remapBlock(sc.target_block_id, maps);
    if (sc.low_value)
    {
        result.low_value = remapOperand(*sc.low_value, maps);
    }
    if (sc.high_value)
    {
        result.high_value = remapOperand(*sc.high_value, maps);
    }
    return result;
}

/**
 * Deep-copy and remap all id references inside a PhiIncomingValue.
 *
 * @param piv  PhiIncomingValue to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped PhiIncomingValue with all ids pointing into the merged model.
 */
static Core::PhiIncomingValue remapPhiIncoming(const Core::PhiIncomingValue &piv, const IdMaps &maps)
{
    return Core::PhiIncomingValue{remapBlock(piv.block_id, maps), remapOperand(piv.value, maps)};
}

/**
 * Deep-copy and remap all id references inside a TypeData variant.
 *
 * @param data TypeData variant to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped TypeData with all ids pointing into the merged model.
 */
static Core::TypeData remapTypeData(const Core::TypeData &data, const IdMaps &maps)
{
    return std::visit(overloaded{[&](const Core::UnknownType &u) -> Core::TypeData { return u; },
                                 [&](const Core::VoidType &v) -> Core::TypeData { return v; },
                                 [&](const Core::EnumType &e) -> Core::TypeData { return e; },
                                 [&](const Core::BoolType &b) -> Core::TypeData { return b; },
                                 [&](const Core::FloatType &f) -> Core::TypeData { return f; },
                                 [&](const Core::IntegerType &i) -> Core::TypeData { return i; },
                                 [&](const Core::PointerType &p) -> Core::TypeData {
                                     return Core::PointerType{remapType(p.pointee_type_id, maps), p.is_restrict};
                                 },
                                 [&](const Core::ArrayType &a) -> Core::TypeData {
                                     return Core::ArrayType{remapType(a.element_type_id, maps), a.element_count};
                                 },
                                 [&](const Core::StructType &s) -> Core::TypeData {
                                     Core::StructType result;
                                     for (const auto &fid : s.fields)
                                     {
                                         result.fields.push_back(remapVar(fid, maps));
                                     }
                                     return result;
                                 },
                                 [&](const Core::UnionType &u) -> Core::TypeData {
                                     Core::UnionType result;
                                     for (const auto &fid : u.fields)
                                     {
                                         result.fields.push_back(remapVar(fid, maps));
                                     }
                                     return result;
                                 },
                                 [&](const Core::FunctionType &f) -> Core::TypeData {
                                     Core::FunctionType result;
                                     result.return_type_id = remapType(f.return_type_id, maps);
                                     for (const auto &ptid : f.parameter_type_ids)
                                     {
                                         result.parameter_type_ids.push_back(remapType(ptid, maps));
                                     }
                                     result.is_variadic = f.is_variadic;
                                     return result;
                                 },
                                 [&](const Core::ComplexType &c) -> Core::TypeData {
                                     return Core::ComplexType{remapType(c.component_type_id, maps)};
                                 }},
                      data);
}

/**
 * Deep-copy and remap all id references inside an InstructionData variant.
 *
 * @param data InstructionData variant to remap.
 * @param maps Id translation tables built during the merge.
 *
 * @return Remapped InstructionData with all ids pointing into the merged model.
 */
static Core::InstructionData remapInstrData(const Core::InstructionData &data, const IdMaps &maps)
{
    return std::visit(overloaded{[&](const std::monostate &m) -> Core::InstructionData { return m; },
                                 [&](const Core::AssignInstruction &ai) -> Core::InstructionData {
                                     Core::AssignInstruction result;
                                     result.opcode = ai.opcode;
                                     result.lhs = remapOperand(ai.lhs, maps);
                                     if (ai.rhs1)
                                     {
                                         result.rhs1 = remapOperand(*ai.rhs1, maps);
                                     }
                                     if (ai.rhs2)
                                     {
                                         result.rhs2 = remapOperand(*ai.rhs2, maps);
                                     }
                                     if (ai.rhs3)
                                     {
                                         result.rhs3 = remapOperand(*ai.rhs3, maps);
                                     }

                                     return result;
                                 },
                                 [&](const Core::CallInstruction &ci) -> Core::InstructionData {
                                     Core::CallInstruction result;
                                     if (ci.lhs)
                                     {
                                         result.lhs = remapOperand(*ci.lhs, maps);
                                     }
                                     result.callee = remapOperand(ci.callee, maps);
                                     for (const auto &arg : ci.arguments)
                                     {
                                         result.arguments.push_back(remapOperand(arg, maps));
                                     }
                                     return result;
                                 },
                                 [&](const Core::ReturnInstruction &ri) -> Core::InstructionData {
                                     Core::ReturnInstruction result;
                                     if (ri.return_value)
                                     {
                                         result.return_value = remapOperand(*ri.return_value, maps);
                                     }
                                     return result;
                                 },
                                 [&](const Core::CondInstruction &ci) -> Core::InstructionData {
                                     Core::CondInstruction result;
                                     result.opcode = ci.opcode;
                                     result.lhs = remapOperand(ci.lhs, maps);
                                     result.rhs = remapOperand(ci.rhs, maps);
                                     result.true_target = remapBlock(ci.true_target, maps);
                                     result.false_target = remapBlock(ci.false_target, maps);
                                     return result;
                                 },
                                 [&](const Core::SwitchInstruction &si) -> Core::InstructionData {
                                     Core::SwitchInstruction result;
                                     result.index = remapOperand(si.index, maps);
                                     for (const auto &sc : si.cases)
                                     {
                                         result.cases.push_back(remapSwitchCase(sc, maps));
                                     }
                                     return result;
                                 },
                                 [&](const Core::GotoInstruction &gi) -> Core::InstructionData {
                                     return Core::GotoInstruction{remapBlock(gi.target, maps)};
                                 },
                                 [&](const Core::LabelInstruction &li) -> Core::InstructionData {
                                     return Core::LabelInstruction{remapOperand(li.label, maps)};
                                 },
                                 [&](const Core::AsmInstruction &ai) -> Core::InstructionData {
                                     Core::AsmInstruction result = ai;
                                     for (auto &out : result.outputs)
                                     {
                                         out.operand = remapOperand(out.operand, maps);
                                     }
                                     for (auto &in : result.inputs)
                                     {
                                         in.operand = remapOperand(in.operand, maps);
                                     }
                                     return result;
                                 },
                                 [&](const Core::PhiInstruction &pi) -> Core::InstructionData {
                                     Core::PhiInstruction result;
                                     result.lhs = remapOperand(pi.lhs, maps);
                                     for (const auto &iv : pi.incoming_values)
                                     {
                                         result.incoming_values.push_back(remapPhiIncoming(iv, maps));
                                     }
                                     return result;
                                 },
                                 [&](const Core::ClobberInstruction &ci) -> Core::InstructionData {
                                     return Core::ClobberInstruction{remapOperand(ci.clobbered_variable, maps)};
                                 },
                                 [&](const Core::UnreachableInstruction &u) -> Core::InstructionData { return u; },
                                 [&](const Core::AbortInstruction &a) -> Core::InstructionData { return a; },
                                 [&](const Core::UnknownInstruction &u) -> Core::InstructionData { return u; }},
                      data);
}

/**
 * Compute a deduplication key for a named type.
 *
 * Anonymous types (empty name) return an empty string, which prevents deduplication.
 *
 * @param t Type to derive a key for.
 *
 * @return Stable string key encoding kind, name, size, alignment, and qualifiers.
 */
static std::string typeKey(const Core::Type &t)
{
    if (t.name.empty())
    {
        return {};
    }

    return std::string(toString(t.kind)) + "|" + t.name + "|" + std::to_string(t.size_bits) + "|" +
           std::to_string(t.alignment) + "|" + (t.is_const ? "C" : "") + (t.is_volatile ? "V" : "") +
           (t.is_atomic ? "A" : "");
}

Core::CodeModel ModelMerger::merge(std::vector<Core::CodeModel> models)
{
    Core::CodeModel merged;
    std::vector<IdMaps> id_maps(models.size());

    std::unordered_map<std::string, Core::TypeId> extern_types;
    std::unordered_map<std::string, Core::VariableId> extern_vars;
    std::unordered_map<std::string, Core::FunctionId> extern_funcs;

    struct TypeOrigin
    {
        size_t tu;
        Core::TypeId src_id;
    };
    std::unordered_map<Core::TypeId, TypeOrigin> type_origins;

    // types
    // allocate merged TypeId for every source type
    // dedup named types by surface key
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        IdMaps &maps = id_maps[tu];
        for (const auto &t : models[tu].types())
        {
            std::string key = typeKey(t);
            if (!key.empty())
            {
                auto it = extern_types.find(key);
                if (it != extern_types.end())
                {
                    // reuse existing merged TypeId
                    maps.type_map[t.id] = it->second;
                    continue;
                }
            }

            // new type
            Core::Type *tp = merged.createType();
            *tp = t;
            tp->id = Core::TypeId{merged.types().size() - 1};

            maps.type_map[t.id] = tp->id;
            if (!key.empty())
            {
                extern_types[key] = tp->id;
            }
            type_origins[tp->id] = {tu, t.id};
        }
    }

    // variables
    // import all variables, build var_map
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        IdMaps &maps = id_maps[tu];
        for (const auto &v : models[tu].variables())
        {
            bool is_field = std::holds_alternative<Core::FieldVariable>(v.data);
            if (!is_field)
            {
                const auto &sv = std::get<Core::StandardVariable>(v.data);
                if (sv.linkage == Core::Linkage::EXTERNAL)
                {
                    auto it = extern_vars.find(v.name);
                    if (it != extern_vars.end())
                    {
                        maps.var_map[v.id] = it->second;

                        if (sv.initial_value)
                        {
                            auto *existing = merged.getVariableMutable(it->second);
                            auto &esv = std::get<Core::StandardVariable>(existing->data);
                            if (!esv.initial_value)
                            {
                                esv.initial_value = sv.initial_value;
                            }
                        }

                        continue;
                    }

                    Core::Variable *vp = merged.createVariable();
                    *vp = v;
                    vp->id = Core::VariableId{merged.variables().size() - 1};
                    maps.var_map[v.id] = vp->id;
                    extern_vars[v.name] = vp->id;
                    continue;
                }
            }

            // field var, or INTERNAL/NONE standard var - always import
            Core::Variable *vp = merged.createVariable();
            *vp = v;
            vp->id = Core::VariableId{merged.variables().size() - 1};
            maps.var_map[v.id] = vp->id;
        }
    }

    // remap every canonical type's TypeData using remapTypeData()
    for (auto &[merged_id, origin] : type_origins)
    {
        const auto &src_type = *models[origin.tu].getType(origin.src_id);
        const IdMaps &maps = id_maps[origin.tu];
        Core::Type *mt = merged.getTypeMutable(merged_id);
        mt->data = remapTypeData(src_type.data, maps);
    }

    // remap type_id fields in all imported variables
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        const IdMaps &maps = id_maps[tu];
        for (const auto &v : models[tu].variables())
        {
            Core::VariableId merged_vid = maps.var_map.at(v.id);
            Core::Variable *mv = merged.getVariableMutable(merged_vid);
            mv->type_id = remapType(v.type_id, maps);
            auto *sv = std::get_if<Core::StandardVariable>(&mv->data);
            const auto *src_sv = std::get_if<Core::StandardVariable>(&v.data);
            if (sv && src_sv && src_sv->initial_value)
            {
                sv->initial_value = remapInitializer(*src_sv->initial_value, maps);
            }
        }
    }

    // functions
    // dedup by name, build func_map
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        IdMaps &maps = id_maps[tu];
        for (const auto &f : models[tu].functions())
        {
            auto it = extern_funcs.find(f.name);
            if (it != extern_funcs.end())
            {
                maps.func_map[f.id] = it->second;
                // if existing is a declaration and this is a definition, swap
                Core::Function *existing = merged.getFunctionMutable(it->second);
                bool existing_is_decl = existing->block_ids.empty();
                if (existing_is_decl && !f.block_ids.empty())
                {
                    // adopt this TU's definition
                    existing->return_type_id = remapType(f.return_type_id, maps);
                    existing->parameter_ids.clear();
                    for (const auto &pid : f.parameter_ids)
                    {
                        existing->parameter_ids.push_back(remapVar(pid, maps));
                    }
                    existing->local_variable_ids.clear();
                    for (const auto &lvid : f.local_variable_ids)
                    {
                        existing->local_variable_ids.push_back(remapVar(lvid, maps));
                    }
                    existing->block_ids.clear(); // filled in connectivity fixup
                }

                continue;
            }

            // new function
            Core::Function mf = f;
            mf.return_type_id = remapType(f.return_type_id, maps);
            mf.parameter_ids.clear();
            for (const auto &pid : f.parameter_ids)
            {
                mf.parameter_ids.push_back(remapVar(pid, maps));
            }
            mf.local_variable_ids.clear();
            for (const auto &lvid : f.local_variable_ids)
            {
                mf.local_variable_ids.push_back(remapVar(lvid, maps));
            }
            mf.block_ids.clear(); // filled in connectivity fixup

            Core::Function *fp = merged.createFunction();
            *fp = mf;
            fp->id = Core::FunctionId{merged.functions().size() - 1};
            maps.func_map[f.id] = fp->id;
            extern_funcs[f.name] = fp->id;
        }
    }

    // blocks
    // allocate merged BlockId for every source block, build block_map
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        IdMaps &maps = id_maps[tu];
        for (const auto &b : models[tu].blocks())
        {
            Core::Block mb;
            mb.parent = remapFunc(b.parent, maps);
            mb.name = b.name;
            // predecessors/successors/instruction_ids filled in connectivity fixup
            Core::Block *bp = merged.createBlock();
            *bp = mb;
            bp->id = Core::BlockId{merged.blocks().size() - 1};
            maps.block_map[b.id] = bp->id;
        }
    }

    // instructions
    // import all instructions with remapped data, build instr_map
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        IdMaps &maps = id_maps[tu];
        for (const auto &instr : models[tu].instructions())
        {
            Core::Instruction mi = instr;
            mi.parent_block_id = remapBlock(instr.parent_block_id, maps);
            mi.data = remapInstrData(instr.data, maps);

            Core::Instruction *ip = merged.createInstruction();
            *ip = mi;
            ip->id = Core::InstructionId{merged.instructions().size() - 1};
            maps.instr_map[instr.id] = ip->id;
        }
    }

    // block/function connectivity fixup
    for (size_t tu = 0; tu < models.size(); tu++)
    {
        const IdMaps &maps = id_maps[tu];
        const auto &model = models[tu];

        for (const auto &b : model.blocks())
        {
            Core::Block *mb = merged.getBlockMutable(maps.block_map.at(b.id));
            for (const auto &pred : b.predecessors)
            {
                mb->predecessors.push_back(remapBlock(pred, maps));
            }
            for (const auto &succ : b.successors)
            {
                mb->successors.push_back(remapBlock(succ, maps));
            }
            for (const auto &iid : b.instruction_ids)
            {
                mb->instruction_ids.push_back(remapInstr(iid, maps));
            }
        }

        for (const auto &f : model.functions())
        {
            Core::Function *mf = merged.getFunctionMutable(maps.func_map.at(f.id));
            for (const auto &bid : f.block_ids)
            {
                mf->block_ids.push_back(remapBlock(bid, maps));
            }
        }
    }

    return merged;
}

} // namespace CodeListener::Exporters
