#include <sstream>

#include "DOTExporter.hpp"

namespace CodeListener
{

namespace Exporters
{

DOTExporter::DOTExporter(std::ostream &os) : os(os)
{
}

void DOTExporter::exportModel(const Core::CodeModel &model)
{
    os << "digraph IR {\n";
    os << "    node [shape=none, fontname=\"Courier New\", fontsize=10];\n";
    os << "    edge [fontname=\"Courier New\", fontsize=9];\n";
    os << "    graph [rankdir=TB, splines=polyline];\n\n";

    for (const auto &[id, func] : model.getFunctions())
    {
        exportFunction(model, func);
    }

    os << "}\n";
}

void DOTExporter::exportFunction(const Core::CodeModel &model, const Core::Function &func)
{
    auto func_id = static_cast<uint32_t>(func.id);

    os << "    subgraph cluster_func_" << func_id << " {\n";
    os << "        label=<<b>Function: " << escape(func.name) << "</b>>;\n";
    os << "        style=filled;\n";
    os << "        fillcolor=\"#f8f9fa\";\n";
    os << "        color=\"#6c757d\";\n";
    os << "        margin=15;\n\n";

    for (auto block_id : func.block_ids)
    {
        exportBlock(model, *model.getBlock(block_id));
    }
    os << "    }\n\n";

    for (auto block_id : func.block_ids)
    {
        const auto &block = model.getBlock(block_id);
        if (block->instruction_ids.empty())
        {
            continue;
        }

        auto src_id = static_cast<uint32_t>(block->id);
        bool handled_edges = false;

        const auto &last_instr = model.getInstruction(block->instruction_ids.back());
        if (last_instr->kind == Core::InstructionKind::SWITCH)
        {
            for (const auto &sw_case : last_instr->switch_cases)
            {
                if (sw_case.target_block_id != Core::NodeId::INVALID)
                {
                    std::string label =
                        sw_case.low_value.has_value() ? formatOperand(model, *sw_case.low_value) : "default";
                    os << "    block_" << src_id << " -> block_" << static_cast<uint32_t>(sw_case.target_block_id)
                       << " [label=\"" << escape(label) << "\", color=\"#d97706\", fontcolor=\"#d97706\"];\n";
                }
            }
            handled_edges = true;
        }
        else if (last_instr->kind == Core::InstructionKind::COND)
        {
            if (block->successor_block_ids.size() >= 2)
            {
                os << "    block_" << src_id << " -> block_" << static_cast<uint32_t>(block->successor_block_ids[0])
                   << " [label=\"true\", color=\"#2e7d32\", fontcolor=\"#2e7d32\"];\n"; // Green
                os << "    block_" << src_id << " -> block_" << static_cast<uint32_t>(block->successor_block_ids[1])
                   << " [label=\"false\", color=\"#c62828\", fontcolor=\"#c62828\"];\n"; // Red
                handled_edges = true;
            }
        }

        if (!handled_edges)
        {
            for (auto succ_id : block->successor_block_ids)
            {
                os << "    block_" << src_id << " -> block_" << static_cast<uint32_t>(succ_id)
                   << " [color=\"#495057\"];\n";
            }
        }
    }
}

void DOTExporter::exportBlock(const Core::CodeModel &model, const Core::Block &block)
{
    auto block_id = static_cast<uint32_t>(block.id);

    os << "    block_" << block_id << " [label=<\n";
    os << "        <table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n";

    os << "            <tr><td bgcolor=\"#e9ecef\" colspan=\"2\" align=\"center\">"
       << "<b>Block " << escape(block.name) << " (" << block_id << ")</b></td></tr>\n";

    if (block.instruction_ids.empty())
    {
        os << "            <tr><td bgcolor=\"#ffffff\" colspan=\"2\"><i>&lt;empty&gt;</i></td></tr>\n";
    }
    else
    {
        for (auto instr_id : block.instruction_ids)
        {
            os << exportInstruction(model, *model.getInstruction(instr_id));
        }
    }

    os << "        </table>\n";
    os << "    >];\n";
}

std::string DOTExporter::exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    std::ostringstream ss;

    std::string bgcolor = "#ffffff";

    switch (instr.kind)
    {
    case Core::InstructionKind::ASSIGN:
        bgcolor = "#e8f5e9";
        break;
    case Core::InstructionKind::CALL:
        bgcolor = "#e3f2fd";
        break;
    case Core::InstructionKind::RETURN:
        bgcolor = "#e1bee7";
        break;
    case Core::InstructionKind::COND:
        bgcolor = "#fff8e1";
        break;
    case Core::InstructionKind::SWITCH:
        bgcolor = "#fff3e0";
        break;
    case Core::InstructionKind::GOTO:
        bgcolor = "#f3e5f5";
        break;
    case Core::InstructionKind::PHI:
        bgcolor = "#fce4ec";
        break;
    case Core::InstructionKind::ABORT:
        bgcolor = "#ef9a9a";
        break;
    case Core::InstructionKind::LABEL:
        bgcolor = "#cfd8dc";
        break;
    case Core::InstructionKind::ASM:
        bgcolor = "#d7ccc8";
        break;
    case Core::InstructionKind::NOP:
        bgcolor = "#f5f5f5";
        break;
    case Core::InstructionKind::CLOBBER:
        bgcolor = "#ffebee";
        break;
    case Core::InstructionKind::UNREACHABLE:
        bgcolor = "#bcaaa4";
        break;
    case Core::InstructionKind::UNKNOWN:
        bgcolor = "#eeeeee";
        break;
    default:
        bgcolor = "#ffffff";
        break;
    }

    std::string readable_expr = formatInstructionText(model, instr);

    std::string tooltip_attr = "";
    if (!instr.source_location.file.empty() && instr.source_location.file != "<unknown>")
    {
        // TODO: let user click to open source file?
        tooltip_attr = " tooltip=\"" + escape(instr.source_location.file) + ":" +
                       std::to_string(instr.source_location.line) + "\" href=\"#\"";
    }

    ss << "            <tr>\n"
       << "                <td bgcolor=\"" << bgcolor << "\" align=\"right\" width=\"30\"" << tooltip_attr << ">"
       << static_cast<uint32_t>(instr.id) << "</td>\n"
       << "                <td bgcolor=\"" << bgcolor << "\" align=\"left\"" << tooltip_attr << ">" << readable_expr
       << "</td>\n"
       << "            </tr>\n";

    return ss.str();
}

std::string DOTExporter::formatOperand(const Core::CodeModel &model, const Core::Operand &op)
{
    if (std::holds_alternative<Core::ConstantOperand>(op))
    {
        return escape(std::get<Core::ConstantOperand>(op).value);
    }
    else if (std::holds_alternative<Core::VariableOperand>(op))
    {
        const auto &varOp = std::get<Core::VariableOperand>(op);

        std::string res = escape(model.getVariable(varOp.variable_id)->name);

        for (const auto &acc : varOp.access_path)
        {
            res = formatAccessor(model, acc, res);
        }
        return res;
    }
    return "???";
}

std::string DOTExporter::formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc,
                                        const std::string &base)
{
    switch (acc.kind)
    {
    case Core::AccessorKind::DEREF:
        return "*(" + base + ")";
    case Core::AccessorKind::ADDRESS_OF:
        return "&amp;(" + base + ")";
    case Core::AccessorKind::FIELD: {
        std::string field_name = escape(model.getVariable(acc.target_field_id)->name);
        return base + "." + field_name;
    }
    case Core::AccessorKind::ARRAY: {
        std::string idx = (acc.index_operand_id != Core::NodeId::INVALID)
                              ? escape(model.getVariable(acc.index_operand_id)->name)
                              : "?";
        return base + "[" + idx + "]";
    }
    case Core::AccessorKind::OFFSET:
        return "(" + base + " + offset)";
    case Core::AccessorKind::BIT_SLICE:
        return base + "[" + std::to_string(acc.bit_start) + ":" + std::to_string(acc.bit_start + acc.bit_size) + "]";
    default:
        return base + "[?]";
    }
}

std::string DOTExporter::escape(const std::string &str)
{
    std::string res;
    res.reserve(str.size());
    for (char c : str)
    {
        switch (c)
        {
        case '<':
            res += "&lt;";
            break;
        case '>':
            res += "&gt;";
            break;
        case '&':
            res += "&amp;";
            break;
        case '"':
            res += "&quot;";
            break;
        case '\n':
            res += "<br/>";
            break;
        default:
            res += c;
            break;
        }
    }
    return res;
}

std::string DOTExporter::opCodeToString(Core::OpCode opcode)
{
    switch (opcode)
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
        return "&amp;";
    case Core::OpCode::BIT_OR:
        return "|";
    case Core::OpCode::BIT_XOR:
        return "^";
    case Core::OpCode::SHL:
        return "&lt;&lt;";
    case Core::OpCode::SHR:
        return "&gt;&gt;";
    case Core::OpCode::LOG_AND:
        return "&amp;&amp;";
    case Core::OpCode::LOG_OR:
        return "||";
    case Core::OpCode::EQUAL:
        return "==";
    case Core::OpCode::NOT_EQUAL:
        return "!=";
    case Core::OpCode::GREATER_THAN:
        return "&gt;";
    case Core::OpCode::LESS_THAN:
        return "&lt;";
    case Core::OpCode::GREATER_EQUAL:
        return "&gt;=";
    case Core::OpCode::LESS_EQUAL:
        return "&lt;=";
    case Core::OpCode::NEGATE:
        return "-";
    case Core::OpCode::BIT_NOT:
        return "~";
    case Core::OpCode::LOG_NOT:
        return "!";
    case Core::OpCode::ADDRESS_OF:
        return "&amp;";
    case Core::OpCode::CAST:
        return "(cast)";
    default:
        return "";
    }
}

std::string DOTExporter::formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr)
{
    std::vector<std::string> ops;
    ops.reserve(instr.operands.size());
    for (const auto &op : instr.operands)
    {
        ops.push_back(formatOperand(model, op));
    }

    std::string op_str = opCodeToString(instr.opcode);

    switch (instr.kind)
    {
    case Core::InstructionKind::ASSIGN: {
        std::string lhs = ops.empty() ? "??" : ops[0];
        std::string type_str = "";
        std::string cast_str = "";

        if (!instr.operands.empty())
        {
            type_str = getOperandTypeString(model, instr.operands[0]);
            if (!type_str.empty())
            {
                cast_str = "(" + type_str + ") ";
                type_str = "<font color=\"#2e7d32\">" + type_str + "</font> ";
            }
        }

        if (instr.opcode == Core::OpCode::CAST && ops.size() == 2)
        {
            return "<b>" + type_str + lhs + "</b> = " + cast_str + ops[1];
        }

        if (ops.size() == 3)
        {
            return "<b>" + type_str + lhs + "</b> = " + ops[1] + " " + op_str + " " + ops[2];
        }
        else if (ops.size() == 2)
        {
            if (instr.opcode != Core::OpCode::NONE)
            {
                return "<b>" + type_str + lhs + "</b> = " + op_str + ops[1];
            }
            return "<b>" + type_str + lhs + "</b> = " + ops[1];
        }
        break;
    }

    case Core::InstructionKind::CLOBBER: {
        std::string kind_name = escape(toString(instr.kind));

        for (auto &c : kind_name)
        {
            c = std::tolower(c);
        }

        return "<b><font color=\"#c62828\">" + kind_name + "</font></b> " + (ops.empty() ? "" : ops[0]);
    }

    case Core::InstructionKind::COND:
        if (ops.size() == 2)
        {
            return "<b>if</b> (" + ops[0] + " " + op_str + " " + ops[1] + ")";
        }
        else if (ops.size() == 1)
        {
            return "<b>if</b> (" + ops[0] + ")";
        }
        break;

    case Core::InstructionKind::CALL: {
        std::string res;
        size_t start_idx = 0;

        if (!ops.empty() && std::holds_alternative<Core::VariableOperand>(instr.operands[0]))
        {
            std::string ret_type = getOperandTypeString(model, instr.operands[0]);
            if (!ret_type.empty())
            {
                res += "<b><font color=\"#2e7d32\">" + ret_type + "</font> " + ops[0] + "</b> = ";
            }
            else
            {
                res += "<b>" + ops[0] + "</b> = ";
            }
            start_idx = 1;
        }

        res += "<b>call</b> " + (start_idx < ops.size() ? ops[start_idx] : "??") + "(";
        for (size_t i = start_idx + 1; i < ops.size(); ++i)
        {
            res += ops[i] + (i + 1 == ops.size() ? "" : ", ");
        }
        res += ")";
        return res;
    }

    case Core::InstructionKind::RETURN:
        return "<b>return</b> " + (ops.empty() ? "" : ops[0]);

    case Core::InstructionKind::GOTO:
        return "<b>goto</b> " + (ops.empty() ? "??" : ops[0]);

    case Core::InstructionKind::LABEL:
        return "<b>" + (ops.empty() ? "label" : ops[0]) + ":</b>";

    case Core::InstructionKind::PHI: {
        std::string type_str = "";
        if (!instr.operands.empty())
        {
            type_str = getOperandTypeString(model, instr.operands[0]);
            if (!type_str.empty())
            {
                type_str = "<font color=\"#2e7d32\">" + type_str + "</font> ";
            }
        }

        std::string res = "<b>" + type_str + (ops.empty() ? "??" : ops[0]) + "</b> = <b>phi</b>(";
        for (size_t i = 1; i < ops.size(); ++i)
        {
            res += ops[i] + (i + 1 == ops.size() ? "" : ", ");
        }
        res += ")";
        return res;
    }

    case Core::InstructionKind::ABORT:
        return "<b>abort()</b>";
    case Core::InstructionKind::NOP:
        return "<b>nop</b>";
    case Core::InstructionKind::UNREACHABLE:
        return "<b>unreachable</b>";
    case Core::InstructionKind::ASM:
        return "<b>asm(...)</b>";

    default:
        break;
    }

    std::string fallback = "<b>" + escape(toString(instr.kind)) + "</b> ";
    for (size_t i = 0; i < ops.size(); ++i)
    {
        fallback += ops[i] + (i + 1 == ops.size() ? "" : ", ");
    }

    return fallback;
}

std::string DOTExporter::getOperandTypeString(const Core::CodeModel &model, const Core::Operand &op)
{
    if (std::holds_alternative<Core::VariableOperand>(op))
    {
        auto var_id = std::get<Core::VariableOperand>(op).variable_id;
        auto type_id = model.getVariable(var_id)->type_id;

        if (type_id != Core::NodeId::INVALID)
        {
            std::string type_name = model.getType(type_id)->name;
            return escape(type_name);
        }
    }
    return "";
}

} // namespace Exporters

} // namespace CodeListener
