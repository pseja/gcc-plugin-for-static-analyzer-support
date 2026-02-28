#include <nlohmann/json.hpp>

#include "JSONExporter.hpp"
#include "Accessor.hpp"
#include "AccessorKind.hpp"
#include "Block.hpp"
#include "ConstantOperand.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "InstructionKind.hpp"
#include "Linkage.hpp"
#include "OpCode.hpp"
#include "Scope.hpp"
#include "SourceLocation.hpp"
#include "StorageDuration.hpp"
#include "SwitchCase.hpp"
#include "Type.hpp"
#include "TypeKind.hpp"
#include "Variable.hpp"
#include "VariableOperand.hpp"

using json = nlohmann::json;

namespace CodeListener
{

namespace Core
{

void to_json(json &j, const SourceLocation &loc)
{
    j = json{{"file", loc.file}, {"line", loc.line}, {"column", loc.column}, {"function", loc.function}};
}


void to_json(json &j, const Variable &var)
{
    j = json{{"id", var.id},
             {"name", var.name},
             {"type_id", var.type_id},
             {"location", var.source_location},
             {"scope", toString(var.scope)},
             {"storage_duration", toString(var.storage_duration)},
             {"linkage", toString(var.linkage)},
             {"is_bitfield", var.is_bitfield}};
    if (var.is_bitfield)
    {
        j["bitfield_size"] = var.bitfield_size;
        j["bitfield_offset"] = var.bitfield_offset;
    }
}

void to_json(json &j, const Type &type)
{
    j = json{{"id", type.id},
             {"kind", toString(type.kind)},
             {"name", type.name},
             {"size_bits", type.size_bits},
             {"size_bytes", type.size_bytes},
             {"alignment", type.alignment},
             {"is_const", type.is_const},
             {"is_volatile", type.is_volatile},
             {"is_restrict", type.is_restrict},
             {"is_atomic", type.is_atomic},
             {"is_unsigned", type.is_unsigned},
             {"is_struct", type.is_struct},
             {"is_union", type.is_union}};

    if (type.array_element_count > 0)
    {
        j["array_element_count"] = type.array_element_count;
    }

    std::vector<int64_t> nested_ids;
    for (auto nid : type.nested_type_ids)
    {
        nested_ids.push_back((int64_t)nid);
    }
    j["nested_type_ids"] = nested_ids;
}

void to_json(json &j, const Accessor &acc)
{
    j = json{{"kind", toString(acc.kind)}};
    if (acc.kind == AccessorKind::FIELD)
    {
        j["target_field_id"] = acc.target_field_id;
    }
    else if (acc.kind == AccessorKind::ARRAY)
    {
        j["index_operand_id"] = acc.index_operand_id;
    }
}

void to_json(json &j, const Operand &op)
{
    if (std::holds_alternative<ConstantOperand>(op))
    {
        const auto &co = std::get<ConstantOperand>(op);
        j = json{{"type", "constant"}, {"id", co.id}, {"value", co.value}};
    }
    else if (std::holds_alternative<VariableOperand>(op))
    {
        const auto &vo = std::get<VariableOperand>(op);
        j = json{{"type", "variable"}, {"variable_id", vo.variable_id}, {"access_path", vo.access_path}};
    }
}

void to_json(json &j, const SwitchCase &sc)
{
    j = json{{"target_block_id", sc.target_block_id}};
    
    if (sc.low_value.has_value())
    {
        if (std::holds_alternative<ConstantOperand>(*sc.low_value))
        {
            const auto &co = std::get<ConstantOperand>(*sc.low_value);
            j["low_value"] = json{{"type", "constant"}, {"id", co.id}, {"value", co.value}};
        }
        else if (std::holds_alternative<VariableOperand>(*sc.low_value))
        {
            const auto &vo = std::get<VariableOperand>(*sc.low_value);
            j["low_value"] = json{{"type", "variable"}, {"variable_id", vo.variable_id}, {"access_path", vo.access_path}};
        }
    }
    else
    {
        j["is_default"] = true;
    }

    if (sc.high_value.has_value())
    {
        if (std::holds_alternative<ConstantOperand>(*sc.high_value))
        {
            const auto &co = std::get<ConstantOperand>(*sc.high_value);
            j["high_value"] = json{{"type", "constant"}, {"id", co.id}, {"value", co.value}};
        }
        else if (std::holds_alternative<VariableOperand>(*sc.high_value))
        {
            const auto &vo = std::get<VariableOperand>(*sc.high_value);
            j["high_value"] = json{{"type", "variable"}, {"variable_id", vo.variable_id}, {"access_path", vo.access_path}};
        }
    }
}

void to_json(json &j, const Instruction &instr)
{
    j = json{{"id", instr.id},
             {"kind", toString(instr.kind)},
             {"opcode", toString(instr.opcode)},
             {"opcode_name", instr.opcode_name},
             {"location", instr.source_location},
             {"operands", instr.operands},
             {"is_terminator", instr.is_terminator}};

    if (!instr.switch_cases.empty())
    {
        j["switch_cases"] = instr.switch_cases;
    }
}

void to_json(json &j, const Block &block)
{
    std::vector<int64_t> preds, succs, instrs;
    for (auto id : block.predecesor_block_ids)
    {
        preds.push_back((int64_t)id);
    }
    for (auto id : block.successor_block_ids)
    {
        succs.push_back((int64_t)id);
    }
    for (auto id : block.instruction_ids)
    {
        instrs.push_back((int64_t)id);
    }

    j = json{{"id", block.id},
             {"name", block.name},
             {"predecessors", preds},
             {"successors", succs},
             {"instruction_ids", instrs}};
}

void to_json(json &j, const Function &func)
{
    std::vector<int64_t> params, locals, blocks;
    for (auto id : func.parameter_ids)
    {
        params.push_back((int64_t)id);
    }
    for (auto id : func.local_variable_ids)
    {
        locals.push_back((int64_t)id);
    }
    for (auto id : func.block_ids)
    {
        blocks.push_back((int64_t)id);
    }

    j = json{{"id", func.id},
             {"name", func.name},
             {"return_type_id", func.return_type_id},
             {"parameter_ids", params},
             {"local_variable_ids", locals},
             {"block_ids", blocks}};
}

} // namespace Core

namespace Exporters
{

JSONExporter::JSONExporter(std::ostream &os) : os(os)
{
}

JSONExporter::JSONExporter(const std::string &filepath) : file_os(filepath), os(file_os)
{
}

void JSONExporter::exportModel(const Core::CodeModel &model)
{
    json j_model;

    std::vector<Core::Type> types;
    for (const auto &[id, t] : model.getTypes())
    {
        types.push_back(t);
    }
    j_model["types"] = types;

    std::vector<Core::Variable> vars;
    for (const auto &[id, v] : model.getVariables())
    {
        vars.push_back(v);
    }
    j_model["variables"] = vars;

    std::vector<Core::Function> funcs;
    for (const auto &[id, f] : model.getFunctions())
    {
        funcs.push_back(f);
    }
    j_model["functions"] = funcs;

    std::vector<Core::Block> blocks;
    for (const auto &[id, b] : model.getBlocks())
    {
        blocks.push_back(b);
    }
    j_model["blocks"] = blocks;

    std::vector<Core::Instruction> instrs;
    for (const auto &[id, i] : model.getInstructions())
    {
        instrs.push_back(i);
    }
    j_model["instructions"] = instrs;

    os << j_model.dump(4) << "\n";
}

} // namespace Exporters

} // namespace CodeListener
