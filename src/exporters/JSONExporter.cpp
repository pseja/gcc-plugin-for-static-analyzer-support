#include <nlohmann/json.hpp>

#include "JSONExporter.hpp"
#include "AccessorKind.hpp"
#include "Block.hpp"
#include "CallGraph.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "SourceLocation.hpp"
#include "SwitchCase.hpp"
#include "Type.hpp"
#include "TypeKind.hpp"
#include "ComplexType.hpp"
#include "utility.hpp"
#include "Variable.hpp"

using json = nlohmann::json;

namespace CodeListener
{

namespace Core
{

template <typename T>
void to_json(json &j, const Id<T> &id)
{
    if (id.isValid())
    {
        j = id.index;
    }
    else
    {
        j = nullptr;
    }
}

void to_json(json &j, const SourceLocation &loc);
void to_json(json &j, const Type &type);
void to_json(json &j, const Accessor &acc);
void to_json(json &j, const Operand &op);
void to_json(json &j, const Initializer &init);
void to_json(json &j, const Variable &var);
void to_json(json &j, const SwitchCase &sc);
void to_json(json &j, const PhiIncomingValue &val);

void to_json(json &j, const Instruction &instr);
void to_json(json &j, const Block &block);
void to_json(json &j, const Function &func);

void to_json(json &j, const SourceLocation &loc)
{
    j = json{{"file", loc.file}, {"line", loc.line}, {"column", loc.column}, {"function", loc.function}};
}

void to_json(json &j, const Type &type)
{
    j = json{{"id", type.id},
             {"name", type.name},
             {"kind", toString(type.kind)},
             {"size_bits", type.size_bits},
             {"alignment", type.alignment},
             {"is_const", type.is_const},
             {"is_volatile", type.is_volatile},
             {"is_atomic", type.is_atomic}};

    std::visit(Exporters::overloaded{[&](const UnknownType &) {}, [&](const VoidType &) {}, [&](const EnumType &) {},
                                     [&](const FloatType &) {}, [&](const BoolType &) {},
                                     [&](const IntegerType &i) { j["is_unsigned"] = i.is_unsigned; },
                                     [&](const PointerType &p) {
                                         j["pointee_type_id"] = p.pointee_type_id;
                                         j["is_restrict"] = p.is_restrict;
                                     },
                                     [&](const ArrayType &a) {
                                         j["element_type_id"] = a.element_type_id;
                                         if (a.element_count)
                                         {
                                             j["element_count"] = a.element_count.value();
                                         }
                                     },
                                     [&](const StructType &s) { j["fields"] = s.fields; },
                                     [&](const UnionType &u) { j["fields"] = u.fields; },
                                     [&](const FunctionType &f) {
                                         j["return_type_id"] = f.return_type_id;
                                         j["parameter_type_ids"] = f.parameter_type_ids;
                                         j["is_variadic"] = f.is_variadic;
                                     },
                                     [&](const ComplexType &c) { j["component_type_id"] = c.component_type_id; }},
               type.data);
}

void to_json(json &j, const Accessor &acc)
{
    j = json{{"kind", toString(acc.kind)}};
    std::visit(Exporters::overloaded{[&](const DerefAccessor &) {},
                                     [&](const ArrayAccessor &a) { j["index"] = a.index; },
                                     [&](const FieldAccessor &f) { j["field_id"] = f.field_id; },
                                     [&](const AddressOfAccessor &a) { j["target_type_id"] = a.target_type_id; },
                                     [&](const OffsetAccessor &o) { j["offset"] = o.offset; },
                                     [&](const BitSliceAccessor &b) {
                                         j["bit_start"] = b.bit_start;
                                         j["bit_size"] = b.bit_size;
                                     }},
               acc.data);
}

void to_json(json &j, const Operand &op)
{
    std::visit(Exporters::overloaded{
                   [&](const ConstantOperand &co) -> void {
                       j = json{{"type", "constant"}, {"type_id", co.type_id}, {"value", co.value}};
                   },
                   [&](const VariableOperand &vo) -> void {
                       j = json{{"type", "variable"}, {"variable_id", vo.id}, {"access_path", vo.access_path}};
                   }},
               op);
}

void to_json(json &j, const Initializer &init)
{
    std::visit(Exporters::overloaded{[&](const Operand &op) -> void { j = op; },
                                     [&](const std::shared_ptr<InitializerList> &list) -> void {
                                         j = json::array();
                                         if (list)
                                         {
                                             for (const auto &elem : list->elements)
                                             {
                                                 j.push_back(elem);
                                             }
                                         }
                                     }},
               init);
}

void to_json(json &j, const Variable &var)
{
    j = json{{"id", var.id},
             {"name", var.name},
             {"type_id", var.type_id},
             {"location", var.source_location},
             {"artificial", var.artificial}};

    std::visit(Exporters::overloaded{[&](const StandardVariable &std_var) {
                                         j["scope"] = toString(std_var.scope);
                                         j["storage_duration"] = toString(std_var.storage_duration);
                                         j["linkage"] = toString(std_var.linkage);
                                         if (std_var.initial_value)
                                         {
                                             j["initial_value"] = std_var.initial_value.value();
                                         }
                                     },
                                     [&](const FieldVariable &field_var) {
                                         j["byte_offset"] = field_var.byte_offset;

                                         if (field_var.bitfield_size)
                                         {
                                             j["bitfield_size"] = field_var.bitfield_size.value();
                                             j["is_bitfield"] = true;
                                         }
                                         if (field_var.bitfield_offset)
                                         {
                                             j["bitfield_offset"] = field_var.bitfield_offset.value();
                                             j["is_bitfield"] = true;
                                         }
                                     }},
               var.data);
}

void to_json(json &j, const SwitchCase &sc)
{
    j = json{{"target_block_id", sc.target_block_id}};
    if (sc.low_value)
    {
        j["low_value"] = sc.low_value.value();
    }
    else
    {
        j["is_default"] = true;
    }
    if (sc.high_value)
    {
        j["high_value"] = sc.high_value.value();
    }
}

void to_json(json &j, const PhiIncomingValue &val)
{
    j = json{{"block_id", val.block_id}, {"value", val.value}};
}

void to_json(json &j, const Instruction &instr)
{
    j = json{{"id", instr.id},
             {"parent_block_id", instr.parent_block_id},
             {"kind", toString(instr.kind)},
             {"is_terminator", instr.is_terminator},
             {"location", instr.source_location}};

    std::visit(Exporters::overloaded{
                   [&](const std::monostate &) { j["data_kind"] = "EMPTY"; },
                   [&](const AssignInstruction &i) {
                       j["opcode"] = toString(i.opcode);
                       j["lhs"] = i.lhs;
                       if (i.rhs1)
                       {
                           j["rhs1"] = i.rhs1.value();
                       }
                       if (i.rhs2)
                       {
                           j["rhs2"] = i.rhs2.value();
                       }
                       if (i.rhs3)
                       {
                           j["rhs3"] = i.rhs3.value();
                       }
                   },
                   [&](const CallInstruction &i) {
                       if (i.lhs)
                       {
                           j["lhs"] = i.lhs.value();
                       }
                       j["callee"] = i.callee;
                       j["arguments"] = i.arguments;
                   },
                   [&](const ReturnInstruction &i) {
                       if (i.return_value)
                       {
                           j["return_value"] = i.return_value.value();
                       }
                   },
                   [&](const CondInstruction &i) {
                       j["opcode"] = toString(i.opcode);
                       j["lhs"] = i.lhs;
                       j["rhs"] = i.rhs;
                       j["true_target"] = i.true_target;
                       j["false_target"] = i.false_target;
                   },
                   [&](const SwitchInstruction &i) {
                       j["index"] = i.index;
                       j["cases"] = i.cases;
                   },
                   [&](const GotoInstruction &i) { j["target"] = i.target; },
                   [&](const LabelInstruction &i) { j["label"] = i.label; },
                   [&](const AsmInstruction &i) {
                       j["assembly_string"] = i.assembly_string;
                       j["is_volatile"] = i.is_volatile;
                       j["outputs"] = json::array();
                       for (const auto &out : i.outputs)
                       {
                           j["outputs"].push_back(json{{"constraint", out.constraint}, {"operand", out.operand}});
                       }
                       j["inputs"] = json::array();
                       for (const auto &in : i.inputs)
                       {
                           j["inputs"].push_back(json{{"constraint", in.constraint}, {"operand", in.operand}});
                       }
                       j["clobbers"] = i.clobbers;
                   },
                   [&](const PhiInstruction &i) {
                       j["lhs"] = i.lhs;
                       j["incoming_values"] = i.incoming_values;
                   },
                   [&](const ClobberInstruction &i) { j["clobbered_variable"] = i.clobbered_variable; },
                   [&](const UnreachableInstruction &) {}, [&](const AbortInstruction &) {},
                   [&](const UnknownInstruction &i) { j["description"] = i.description; }},
               instr.data);
}

void to_json(json &j, const Block &block)
{
    j = json{{"id", block.id},
             {"parent", block.parent},
             {"name", block.name},
             {"predecessors", block.predecessors},
             {"successors", block.successors},
             {"instruction_ids", block.instruction_ids}};
}

void to_json(json &j, const Function &func)
{
    j = json{{"id", func.id},
             {"name", func.name},
             {"return_type_id", func.return_type_id},
             {"source_location", func.source_location},
             {"parameter_ids", func.parameter_ids},
             {"local_variable_ids", func.local_variable_ids},
             {"block_ids", func.block_ids}};
}

} // namespace Core

namespace AnnotationServices
{

void to_json(json &j, const CallGraphEdge &edge);
void to_json(json &j, const CallGraphNode &node);
void to_json(json &j, const CallGraph &cg);

void to_json(json &j, const CallGraphEdge &edge)
{
    j = json{{"call_instruction", edge.call_instruction}};
    if (edge.callee)
        j["callee"] = *edge.callee;
    else
        j["callee"] = nullptr;
}

void to_json(json &j, const CallGraphNode &node)
{
    j = json{{"function_id", node.function_id},
             {"outgoing_calls", node.outgoing_calls},
             {"incoming_calls", node.incoming_calls},
             {"address_taken_at", node.address_taken_at}};
}

void to_json(json &j, const CallGraph &cg)
{
    j = json::object();

    j["nodes"] = json::array();
    for (const auto &[func_id, node] : cg.nodes)
    {
        j["nodes"].push_back(node);
    }

    j["roots"] = cg.roots;
    j["leaves"] = cg.leaves;
    j["topological_order"] = cg.topological_order;
    j["has_indirect_calls"] = cg.has_indirect_calls;
    j["has_callbacks"] = cg.has_callbacks;
}

} // namespace AnnotationServices

namespace Exporters
{

JSONExporter::JSONExporter(std::ostream &os) : os(os)
{
}

JSONExporter::JSONExporter(const std::string &filepath) : file_os(filepath), os(file_os)
{
}

void JSONExporter::onEndModel(const Core::CodeModel &model)
{
    json j_model;

    j_model["types"] = model.types();
    j_model["variables"] = model.variables();
    j_model["functions"] = model.functions();
    j_model["blocks"] = model.blocks();
    j_model["instructions"] = model.instructions();

    json j_annotations = json::object();
    const auto &call_graph = analysis_manager.getAnnotation<AnnotationServices::CallGraph>(model);
    j_annotations["call_graph"] = call_graph;
    j_model["annotations"] = j_annotations;

    os << j_model.dump(4) << "\n";
}

} // namespace Exporters

} // namespace CodeListener