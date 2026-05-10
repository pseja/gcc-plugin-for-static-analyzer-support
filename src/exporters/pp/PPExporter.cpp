/**
 * @file PPExporter.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the pretty-printer that renders the CodeModel as three-address code.
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

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <variant>

#include "AbortInstruction.hpp"
#include "AsmInstruction.hpp"
#include "AssignInstruction.hpp"
#include "Block.hpp"
#include "BoolType.hpp"
#include "CallInstruction.hpp"
#include "ClobberInstruction.hpp"
#include "CodeModel.hpp"
#include "CondInstruction.hpp"
#include "EnumType.hpp"
#include "FieldVariable.hpp"
#include "FloatType.hpp"
#include "Function.hpp"
#include "FunctionType.hpp"
#include "GotoInstruction.hpp"
#include "IntegerType.hpp"
#include "Instruction.hpp"
#include "LabelInstruction.hpp"
#include "PhiInstruction.hpp"
#include "PointerType.hpp"
#include "PPExporter.hpp"
#include "ReturnInstruction.hpp"
#include "Scope.hpp"
#include "StandardVariable.hpp"
#include "SwitchInstruction.hpp"
#include "SwitchToIf.hpp"
#include "Type.hpp"
#include "UnknownInstruction.hpp"
#include "UnreachableInstruction.hpp"
#include "Variable.hpp"
#include "utility.hpp"

namespace CodeListener::Exporters
{

PPExporter::PPExporter(std::ostream &os) : os(os)
{
}

PPExporter::PPExporter(const std::string &filepath) : file_os(filepath), os(file_os)
{
}

void PPExporter::onBeginFunction(const Core::CodeModel &model, const Core::Function &func)
{
    // print function header: name(%arg1: VAR, %arg2: VAR, ...):
    os << func.name << "(";
    for (std::size_t i = 0; i < func.parameter_ids.size(); ++i)
    {
        if (i > 0)
        {
            os << ", ";
        }
        os << "%arg" << (i + 1) << ": ";
        Core::VariableOperand vo{func.parameter_ids[i], {}, std::nullopt};
        Core::Operand op = vo;
        os << fmtOperand(op, model);
    }
    os << "):\n";

    // print entry goto: \t\tgoto FIRST_REAL_BLOCK
    if (!func.block_ids.empty())
    {
        const auto *entry = model.getBlock(func.block_ids.front());
        std::string first_label;

        if (entry && entry->name == "ENTRY" && !entry->successors.empty())
        {
            const auto *succ = model.getBlock(entry->successors.front());
            first_label = succ ? succ->name : entry->name;
        }
        else if (entry)
        {
            first_label = entry->name;
        }

        if (!first_label.empty())
        {
            os << "\t\tgoto " << first_label << "\n";
        }
    }
}

void PPExporter::onEndFunction(const Core::CodeModel &, const Core::Function &)
{
    os << "\n";
}

bool PPExporter::shouldVisitBlock(const Core::CodeModel &, const Core::Block &block)
{
    return block.name != "ENTRY" && block.name != "EXIT";
}

void PPExporter::onBeginBlock(const Core::CodeModel &, const Core::Block &block)
{
    block_has_terminator = false;
    os << "\n\t" << block.name << ":\n";
}

void PPExporter::onEndBlock(const Core::CodeModel &model, const Core::Block &block)
{
    if (block_has_terminator)
    {
        return;
    }

    // block has no explicit terminator: emit implicit fallthrough goto or ret
    for (const auto &succ_id : block.successors)
    {
        const auto *succ = model.getBlock(succ_id);
        if (!succ)
        {
            continue;
        }

        if (succ->name == "EXIT")
        {
            os << "\t\tret\n";
        }
        else
        {
            os << "\t\tgoto " << succ->name << "\n";
        }
        break; // only the first successor
    }
}

void PPExporter::onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    std::visit(
        overloaded{
            [&](const Core::AssignInstruction &a) {
                if (!a.rhs1.has_value())
                {
                    return;
                }

                std::string lhs = fmtOperand(a.lhs, model);
                std::string rhs1 = fmtOperand(a.rhs1.value(), model);

                if (a.rhs2.has_value() && isBinOp(a.opcode))
                {
                    // binary: LHS := (SRC1 OP SRC2)
                    std::string rhs2 = fmtOperand(a.rhs2.value(), model);
                    os << "\t\t" << lhs << " := (" << rhs1 << " " << binopSym(a.opcode) << " " << rhs2 << ")\n";
                }
                else
                {
                    // unary / assign / cast
                    os << "\t\t" << lhs << " := ";
                    switch (a.opcode)
                    {
                    case Core::OpCode::NONE:
                        os << rhs1 << "\n";
                        break;
                    case Core::OpCode::NEGATE:
                        os << "-" << rhs1 << "\n";
                        break;
                    case Core::OpCode::BIT_NOT:
                        os << "~" << rhs1 << "\n";
                        break;
                    case Core::OpCode::LOG_NOT:
                        os << "!" << rhs1 << "\n";
                        break;
                    case Core::OpCode::ABS:
                        os << "abs(" << rhs1 << ")\n";
                        break;
                    case Core::OpCode::CAST: {
                        // determine if this is an int->float cast (CL_UNOP_FLOAT)
                        bool is_float_cast = false;
                        const auto *lhs_var = std::get_if<Core::VariableOperand>(&a.lhs);
                        const auto *rhs_var = std::get_if<Core::VariableOperand>(&a.rhs1.value());
                        if (lhs_var && rhs_var)
                        {
                            const auto *lv = model.getVariable(lhs_var->id);
                            const auto *rv = model.getVariable(rhs_var->id);
                            if (lv && rv)
                            {
                                const auto *lt = model.getType(lv->type_id);
                                const auto *rt = model.getType(rv->type_id);
                                if (lt && rt && lt->kind == Core::TypeKind::REAL && rt->kind == Core::TypeKind::INTEGER)
                                {
                                    is_float_cast = true;
                                }
                            }
                        }
                        else if (lhs_var)
                        {
                            const auto *lv = model.getVariable(lhs_var->id);
                            const auto *rhs_cst = std::get_if<Core::ConstantOperand>(&a.rhs1.value());
                            if (lv && rhs_cst)
                            {
                                const auto *lt = model.getType(lv->type_id);
                                const auto *rt = model.getType(rhs_cst->type_id);
                                if (lt && rt && lt->kind == Core::TypeKind::REAL && rt->kind == Core::TypeKind::INTEGER)
                                {
                                    is_float_cast = true;
                                }
                            }
                        }
                        if (is_float_cast)
                        {
                            os << "(float)" << rhs1 << "\n";
                        }
                        else
                        {
                            os << rhs1 << "\n";
                        }
                        break;
                    }
                    default:
                        os << rhs1 << "\n";
                        break;
                    }
                }
            },
            [&](const Core::CallInstruction &c) {
                os << "\t\t";
                if (c.lhs.has_value())
                {
                    os << fmtOperand(c.lhs.value(), model) << " := ";
                }
                os << fmtOperand(c.callee, model) << "(";
                for (std::size_t i = 0; i < c.arguments.size(); ++i)
                {
                    if (i > 0)
                    {
                        os << ", ";
                    }
                    os << fmtOperand(c.arguments[i], model);
                }
                os << ")\n";
            },
            [&](const Core::GotoInstruction &g) {
                block_has_terminator = true;
                const auto *target = model.getBlock(g.target);
                os << "\t\tgoto " << (target ? target->name : "L_unknown") << "\n";
            },
            [&](const Core::CondInstruction &cond) {
                block_has_terminator = true;
                // if cond has an opcode, emit: TMP := (LHS OP RHS); if (TMP) ...
                // if opcode==NONE, just: if (LHS) ...
                if (cond.opcode != Core::OpCode::NONE)
                {
                    const auto &norm = analysis_manager.getAnnotation<AnnotationServices::SwitchToIf>(model);
                    const std::string &tmp = norm.conds.at(instr.id).synth_comp_temp;
                    std::string lhs = fmtOperand(cond.lhs, model);
                    std::string rhs = fmtOperand(cond.rhs, model);
                    os << "\t\t" << tmp << " := (" << lhs << " " << binopSym(cond.opcode) << " " << rhs << ")\n";
                    os << "\t\tif (" << tmp << ")\n";
                }
                else
                {
                    os << "\t\tif (" << fmtOperand(cond.lhs, model) << ")\n";
                }
                const auto *true_bb = model.getBlock(cond.true_target);
                const auto *false_bb = model.getBlock(cond.false_target);
                os << "\t\t\tgoto " << (true_bb ? true_bb->name : "L_unknown") << "\n";
                os << "\t\telse\n";
                os << "\t\t\tgoto " << (false_bb ? false_bb->name : "L_unknown") << "\n";
            },
            [&](const Core::ReturnInstruction &ret) {
                block_has_terminator = true;
                os << "\t\tret";
                if (ret.return_value.has_value())
                {
                    os << " " << fmtOperand(ret.return_value.value(), model);
                }
                os << "\n";
            },
            [&](const Core::SwitchInstruction &sw) {
                block_has_terminator = true;
                emitSwitchUnfolded(sw, instr.id, model);
            },
            [&](const Core::AbortInstruction &) {
                block_has_terminator = true;
                os << "\t\tabort\n";
            },
            [&](const Core::ClobberInstruction &clob) {
                os << "\t\tCLOBBER " << fmtOperand(clob.clobbered_variable, model) << "\n";
            },
            [&](const Core::LabelInstruction &lbl) {
                // inline label (not a basic-block header)
                // in old pipeline: get_decl_name() returns null for compiler-generated labels, so only user-defined
                // labels print
                const auto *vo = std::get_if<Core::VariableOperand>(&lbl.label);
                if (vo)
                {
                    const auto *var = model.getVariable(vo->id);
                    if (var)
                    {
                        const std::string &name = var->name;
                        // skip compiler-generated labels (L<digits> pattern)
                        bool is_compiler_generated =
                            !name.empty() && name[0] == 'L' && name.size() > 1 &&
                            std::all_of(name.begin() + 1, name.end(), [](char c) { return std::isdigit(c) != 0; });
                        if (!is_compiler_generated)
                        {
                            os << "\t" << name << ":\n";
                        }
                    }
                }
            },
            [&](const Core::UnreachableInstruction &) {
                // treated as abort in the old pp format
                os << "\t\tabort\n";
            },
            [&](const Core::PhiInstruction &) { /* PHI nodes: skip */ },
            [&](const Core::AsmInstruction &) { /* inline asm: skip */ },
            [&](const Core::UnknownInstruction &) { /* unknown: skip */ }, [&](const std::monostate &) {}},
        instr.data);
}

void PPExporter::onEndModel(const Core::CodeModel & /*model*/)
{
    os << "\n"; // file_close blank line
}

void PPExporter::emitSwitchUnfolded(const Core::SwitchInstruction &sw, Core::InstructionId inst_id,
                                    const Core::CodeModel &model)
{
    const std::string index_str = fmtOperand(sw.index, model);
    const auto &norm = analysis_manager.getAnnotation<AnnotationServices::SwitchToIf>(model);
    const auto &data = norm.switches.at(inst_id);

    if (data.checks.empty())
    {
        if (!data.default_target_block_name.empty())
        {
            os << "\t\tgoto " << data.default_target_block_name << "\n";
        }
        return;
    }

    const int base = data.synth_label_base;

    for (std::size_t i = 0; i < data.checks.size(); ++i)
    {
        if (i > 0)
        {
            os << "\n\tL" << (base + static_cast<int>(i) - 1) << ":\n";
        }

        const std::string else_target = "L" + std::to_string(base + static_cast<int>(i));

        os << "\t\t" << data.synth_comp_temp << " := (" << index_str << " == " << data.checks[i].formatted_case_constant
           << ")\n";
        os << "\t\tif (" << data.synth_comp_temp << ")\n";
        os << "\t\t\tgoto " << data.checks[i].target_block_name << "\n";
        os << "\t\telse\n";
        os << "\t\t\tgoto " << else_target << "\n";
    }

    // final empty block -> goto default (matches old emitDefault())
    os << "\n\tL" << (base + static_cast<int>(data.checks.size()) - 1) << ":\n";
    if (!data.default_target_block_name.empty())
    {
        os << "\t\tgoto " << data.default_target_block_name << "\n";
    }
}

std::string PPExporter::fmtOperand(const Core::Operand &op, const Core::CodeModel &model) const
{
    return std::visit(overloaded{[&](const Core::ConstantOperand &cst) -> std::string { return fmtConst(cst, model); },
                                 [&](const Core::VariableOperand &vo) -> std::string {
                                     const auto *var = model.getVariable(vo.id);
                                     if (!var)
                                     {
                                         return "%unknown";
                                     }

                                     // function-typed variable -> output as function name (constant reference)
                                     const auto *type = model.getType(var->type_id);
                                     if (type && type->kind == Core::TypeKind::FUNCTION)
                                     {
                                         return var->name;
                                     }

                                     return fmtVar(vo.id, vo.access_path, model);
                                 }},
                      op);
}

std::string PPExporter::fmtVar(Core::VariableId id, const std::vector<Core::Accessor> &accessors,
                               const Core::CodeModel &model) const
{
    const auto *var = model.getVariable(id);
    if (!var)
    {
        return "%unknown";
    }

    // strip leading (AddressOf, Deref) pairs: MEM_REF(ADDR_EXPR(v), 0) cancels to a direct variable access
    std::vector<Core::Accessor> eff_storage;
    const std::vector<Core::Accessor> *effp = &accessors;
    {
        std::size_t skip = 0;
        while (skip + 1 < accessors.size() && std::holds_alternative<Core::AddressOfAccessor>(accessors[skip].data) &&
               std::holds_alternative<Core::DerefAccessor>(accessors[skip + 1].data))
        {
            skip += 2;
        }

        if (skip > 0)
        {
            eff_storage.assign(accessors.begin() + skip, accessors.end());
            effp = &eff_storage;
        }
    }
    const std::vector<Core::Accessor> &ac = *effp;

    // build the base variable reference
    bool is_named = (var->name != "<anonymous>" && !var->name.empty() && var->name[0] != '%');

    // scope letter: use G for all (matching PredatorAdapter behaviour)
    char scope_letter = 'G';
    if (const auto *sv = std::get_if<Core::StandardVariable>(&var->data))
    {
        switch (sv->scope)
        {
        case Core::Scope::GLOBAL:
            scope_letter = 'G';
            break;
        case Core::Scope::STATIC:
            scope_letter = 'S';
            break;
        case Core::Scope::FUNCTION:
            scope_letter = 'F';
            break;
        }
    }

    std::string uid_str = std::to_string(var->id.index);

    // check for REF (address-of) accessor at end of chain
    bool has_ref = !ac.empty() && std::holds_alternative<Core::AddressOfAccessor>(ac.back().data);

    // check for leading DEREF
    bool leading_deref = !ac.empty() && std::holds_alternative<Core::DerefAccessor>(ac.front().data);

    // check if deref is followed by field access
    bool deref_then_item = leading_deref && ac.size() > 1 && std::holds_alternative<Core::FieldAccessor>(ac[1].data);

    std::string result;

    // address-of prefix
    if (has_ref)
    {
        result += "&";
    }

    // pointer dereference prefix (only when not followed by a field)
    if (leading_deref && !deref_then_item)
    {
        result += "*";
    }

    // base variable: %mSCOPEUID:name or %rSCOPEUID
    if (is_named)
    {
        result += std::string("%m") + scope_letter + uid_str + ":" + var->name;
    }
    else
    {
        result += std::string("%r") + scope_letter + uid_str;
    }

    // Process accessors
    std::size_t start_idx = leading_deref ? 1 : 0;

    // Arrow or dot
    if (deref_then_item)
    {
        result += "->";
        start_idx = 1; // skip DEREF only; the ITEM at index 1 is printed in the loop
    }
    else if (!ac.empty() && std::holds_alternative<Core::FieldAccessor>(ac[0].data))
    {
        result += ".";
    }

    for (std::size_t i = start_idx; i < ac.size(); ++i)
    {
        const auto &acc = ac[i];
        std::visit(overloaded{[&](const Core::FieldAccessor &fa) {
                                  // merge consecutive FieldAccessors: [+off1]name1.[+off2]name2 into
                                  // [+off1+off2]name1.name2 (matches old cl_pp.cc printRecordAccessor which accumulates
                                  // offsets and joins names with '.')
                                  std::string tag;
                                  int offset = 0;

                                  auto get_field_info = [&](Core::VariableId fid) {
                                      const auto *fvar = model.getVariable(fid);
                                      if (!fvar)
                                      {
                                          return;
                                      }

                                      if (const auto *fv = std::get_if<Core::FieldVariable>(&fvar->data))
                                      {
                                          offset += fv->byte_offset.value_or(0);
                                      }
                                      if (!tag.empty())
                                      {
                                          tag += ".";
                                      }
                                      tag += fvar ? fvar->name : "<anon>";
                                  };

                                  get_field_info(fa.field_id);

                                  // consume additional consecutive FieldAccessors
                                  while (i + 1 < ac.size() &&
                                         std::holds_alternative<Core::FieldAccessor>(ac[i + 1].data))
                                  {
                                      i++;
                                      get_field_info(std::get<Core::FieldAccessor>(ac[i].data).field_id);
                                  }

                                  result += "[+" + std::to_string(offset) + "]" + tag;
                              },
                              [&](const Core::ArrayAccessor &arr) {
                                  // [index]
                                  result += "[" + fmtOperand(arr.index, model) + "]";
                              },
                              [&](const Core::DerefAccessor &) {
                                  // additional deref in the middle of a chain
                                  result += "*";
                              },
                              [&](const Core::AddressOfAccessor &) {
                                  // already handled as prefix; skip here
                              },
                              [&](const Core::OffsetAccessor &off) {
                                  // pointer arithmetic: <+N> notation
                                  const auto *cst = std::get_if<Core::ConstantOperand>(&off.offset);
                                  if (cst)
                                  {
                                      long n = std::strtol(cst->value.c_str(), nullptr, 10);
                                      if (n >= 0)
                                      {
                                          result += "<+" + std::to_string(n) + ">";
                                      }
                                      else
                                      {
                                          result += "<" + std::to_string(n) + ">";
                                      }
                                  }
                              },
                              [&](const Core::BitSliceAccessor &) { /* rare; skip */ }},
                   acc.data);
    }

    return result;
}

std::string PPExporter::fmtConst(const Core::ConstantOperand &cst, const Core::CodeModel &model) const
{
    if (cst.value.empty())
    {
        return "";
    }

    const auto *type = model.getType(cst.type_id);
    if (!type)
    {
        // no type info means the GCC adapter could not handle this operand/constant
        // (e.g. complex-number sub-expressions) - old pipeline prints CL_OPERAND_VOID
        return "CL_OPERAND_VOID";
    }

    // string literal: value is stored with surrounding double-quotes but with raw bytes
    // (matches old pipeline's cl_pp.cc prettyEscaped behaviour)
    if (cst.value.size() >= 2 && cst.value.front() == '"')
    {
        std::string out = "\"";
        for (std::size_t i = 1; i + 1 < cst.value.size(); ++i)
        {
            unsigned char c = static_cast<unsigned char>(cst.value[i]);
            if (std::isprint(c) && c != '\'' && c != '"' && c != '\\')
            {
                out += static_cast<char>(c);
            }
            else
            {
                out += '\\';
                switch (c)
                {
                case '\'':
                    out += '\'';
                    break;
                case '"':
                    out += '"';
                    break;
                case '\\':
                    out += '\\';
                    break;
                case '\n':
                    out += 'n';
                    break;
                case '\r':
                    out += 'r';
                    break;
                case '\t':
                    out += 't';
                    break;
                default:
                    out += static_cast<char>('0' + (c >> 6));
                    out += static_cast<char>('0' + ((c & 070u) >> 3));
                    out += static_cast<char>('0' + (c & 007u));
                }
            }
        }
        out += '"';
        return out;
    }

    switch (type->kind)
    {
    case Core::TypeKind::REAL: {
        // format with the same default stream precision (6 sig digits) as old cl_pp.cc
        double val = std::strtod(cst.value.c_str(), nullptr);
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    case Core::TypeKind::BOOL: {
        long val = std::strtol(cst.value.c_str(), nullptr, 10);
        return val ? "true" : "false";
    }

    case Core::TypeKind::POINTER: {
        long val = std::strtol(cst.value.c_str(), nullptr, 10);
        if (val == 0)
        {
            return "NULL";
        }
        std::ostringstream oss;
        oss << "0x" << std::hex << val;
        return oss.str();
    }

    case Core::TypeKind::INTEGER: {
        bool is_unsigned = false;
        if (const auto *it = std::get_if<Core::IntegerType>(&type->data))
        {
            is_unsigned = it->is_unsigned;
        }

        // cl_pp.cc uses (is_unsigned ? (unsigned long)value : value) - the ternary
        // forces both branches to unsigned long, so ALL values print as unsigned.
        // "U" suffix is added only when the type is actually unsigned
        long sval;
        unsigned long uval;
        if (is_unsigned)
        {
            // GCCAdapter stored tree_to_uhwi - value string is unsigned
            uval = std::strtoull(cst.value.c_str(), nullptr, 10);
            sval = static_cast<long>(uval);
        }
        else
        {
            // GCCAdapter stored tree_to_shwi -> value string may be negative
            sval = std::strtol(cst.value.c_str(), nullptr, 10);
            uval = static_cast<unsigned long>(sval);
        }

        std::string result = std::to_string(uval);
        if (is_unsigned)
        {
            result += "U";
        }
        if (sval < 0)
        {
            result = "(" + result + ")";
        }
        return result;
    }

    case Core::TypeKind::ENUM: {
        // CL_TYPE_ENUM falls through to CL_TYPE_INT in cl_pp.cc - same logic
        bool is_unsigned = false;
        if (const auto *et = std::get_if<Core::EnumType>(&type->data))
        {
            is_unsigned = et->is_unsigned;
        }

        long sval;
        unsigned long uval;
        if (is_unsigned)
        {
            uval = std::strtoull(cst.value.c_str(), nullptr, 10);
            sval = static_cast<long>(uval);
        }
        else
        {
            sval = std::strtol(cst.value.c_str(), nullptr, 10);
            uval = static_cast<unsigned long>(sval);
        }

        std::string result = std::to_string(uval);
        if (is_unsigned)
        {
            result += "U";
        }
        if (sval < 0)
        {
            result = "(" + result + ")";
        }
        return result;
    }

    case Core::TypeKind::COMPLEX:
    case Core::TypeKind::UNKNOWN:
        // old pipeline prints CL_OPERAND_VOID for types it cannot handle
        return "CL_OPERAND_VOID";

    default:
        return cst.value;
    }
}

bool PPExporter::isBinOp(Core::OpCode op)
{
    switch (op)
    {
    case Core::OpCode::NONE:
    case Core::OpCode::NEGATE:
    case Core::OpCode::BIT_NOT:
    case Core::OpCode::LOG_NOT:
    case Core::OpCode::ABS:
    case Core::OpCode::CAST:
        return false;
    default:
        return true;
    }
}

std::string PPExporter::binopSym(Core::OpCode op)
{
    switch (op)
    {
    case Core::OpCode::ADD:
        return "+";
    case Core::OpCode::SUB:
        return "-";
    case Core::OpCode::MUL:
        return "*";
    case Core::OpCode::DIV:
        return "/";
    case Core::OpCode::MOD:
        return "%";
    case Core::OpCode::BIT_AND:
        return "&";
    case Core::OpCode::BIT_OR:
        return "|";
    case Core::OpCode::BIT_XOR:
        return "^";
    case Core::OpCode::SHL:
        return "<<";
    case Core::OpCode::SHR:
        return ">>";
    case Core::OpCode::LOG_AND:
        return "and";
    case Core::OpCode::LOG_OR:
        return "or";
    case Core::OpCode::EQUAL:
        return "==";
    case Core::OpCode::NOT_EQUAL:
        return "!=";
    case Core::OpCode::GREATER_THAN:
        return ">";
    case Core::OpCode::LESS_THAN:
        return "<";
    case Core::OpCode::GREATER_EQUAL:
        return ">=";
    case Core::OpCode::LESS_EQUAL:
        return "<=";
    case Core::OpCode::POINTER_ADD:
        return "[ptr]+";
    case Core::OpCode::POINTER_SUB:
        return "[ptr]-";
    case Core::OpCode::MIN:
        return "min";
    case Core::OpCode::MAX:
        return "max";
    case Core::OpCode::ROTATE_LEFT:
        return "L-ROTATE-BY";
    case Core::OpCode::ROTATE_RIGHT:
        return "R-ROTATE-BY";
    default:
        return "(?binop?)";
    }
}

} // namespace CodeListener::Exporters
