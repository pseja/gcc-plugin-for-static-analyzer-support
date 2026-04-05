#include <sstream>

#include "CallGraph.hpp"
#include "DOTExporter.hpp"
#include "OpCode.hpp"
#include "utility.hpp"

namespace CodeListener::Exporters
{

DOTExporter::DOTExporter(std::ostream &os) : os(os)
{
}

DOTExporter::DOTExporter(const std::string &filepath) : file_os(filepath), os(file_os)
{
}

void DOTExporter::onBeginModel(const Core::CodeModel &)
{
    os << "digraph IR {\n";
    os << "    node [shape=none, fontname=\"Courier New\", fontsize=10];\n";
    os << "    edge [fontname=\"Courier New\", fontsize=9];\n";
    os << "    graph [rankdir=TB, splines=polyline, compound=true];\n\n";
}

void DOTExporter::onEndModel(const Core::CodeModel &model)
{
    const auto &cg = analysis_manager.getAnnotation<AnnotationServices::CallGraph>(model);

    for (const auto &[caller_id, node] : cg.nodes)
    {
        for (const auto &edge : node.outgoing_calls)
        {
            if (!edge.callee)
                continue;

            auto callee_id = *edge.callee;
            const auto *callee = model.getFunction(callee_id);
            if (!callee || callee->block_ids.empty())
                continue;

            const auto *instr = model.getInstruction(edge.call_instruction);
            if (!instr)
                continue;

            os << "    block_" << instr->parent_block_id << " -> block_" << callee->block_ids.front()
               << " [lhead=cluster_func_" << callee_id << ", color=\"#6c757d\"];\n";
        }
    }

    os << "}\n";
}

void DOTExporter::onBeginFunction(const Core::CodeModel &, const Core::Function &func)
{
    os << "    subgraph cluster_func_" << func.id << " {\n";
    os << "        label=<<b>Function: " << escape(func.name) << "</b>>;\n";
    os << "        style=filled;\n";
    os << "        fillcolor=\"#f8f9fa\";\n";
    os << "        color=\"#6c757d\";\n";
    os << "        margin=15;\n\n";
}

void DOTExporter::onEndFunction(const Core::CodeModel &, const Core::Function &)
{
    os << "    }\n\n";
}

void DOTExporter::onBeginBlock(const Core::CodeModel &, const Core::Block &block)
{
    os << "        block_" << block.id << " [label=<\n";
    os << "            <table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n";

    os << "                <tr><td bgcolor=\"#e9ecef\" colspan=\"2\" align=\"center\">"
       << "<b>Block " << escape(block.name) << " (" << block.id << ")</b></td></tr>\n";

    if (block.instruction_ids.empty())
    {
        os << "                <tr><td bgcolor=\"#ffffff\" colspan=\"2\"><i>&lt;empty&gt;</i></td></tr>\n";
    }
}

void DOTExporter::onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    os << exportInstruction(model, instr);
}

void DOTExporter::onEndBlock(const Core::CodeModel &model, const Core::Block &block)
{
    os << "            </table>\n";
    os << "        >];\n";
    emitBlockEdges(model, block);
}

void DOTExporter::emitBlockEdges(const Core::CodeModel &model, const Core::Block &block)
{
    auto src_id = block.id;
    bool handled_edges = false;

    if (!block.instruction_ids.empty())
    {
        const auto *last_instr = model.getInstruction(block.instruction_ids.back());

        if (auto *sw_instr = std::get_if<Core::SwitchInstruction>(&last_instr->data))
        {
            for (const auto &sw_case : sw_instr->cases)
            {
                if (sw_case.target_block_id.isValid())
                {
                    std::string label =
                        sw_case.low_value.has_value() ? formatOperand(model, sw_case.low_value.value()) : "default";
                    if (sw_case.high_value.has_value())
                        label += " ... " + formatOperand(model, sw_case.high_value.value());

                    os << "    block_" << src_id << " -> block_" << sw_case.target_block_id << " [label=\""
                       << escape(label) << "\", color=\"#d97706\", fontcolor=\"#d97706\"];\n";
                }
            }
            handled_edges = true;
        }
        else if (auto *cond_instr = std::get_if<Core::CondInstruction>(&last_instr->data))
        {
            if (cond_instr->true_target.isValid())
                os << "    block_" << src_id << " -> block_" << cond_instr->true_target
                   << " [label=\"true\", color=\"#2e7d32\", fontcolor=\"#2e7d32\"];\n";
            if (cond_instr->false_target.isValid())
                os << "    block_" << src_id << " -> block_" << cond_instr->false_target
                   << " [label=\"false\", color=\"#c62828\", fontcolor=\"#c62828\"];\n";
            handled_edges = true;
        }
    }

    if (!handled_edges)
    {
        for (auto succ_id : block.successors)
        {
            os << "    block_" << src_id << " -> block_" << succ_id << " [color=\"#495057\"];\n";
        }
    }
}

std::string DOTExporter::exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    std::ostringstream ss;

    std::string bgcolor = std::visit(overloaded{[](const std::monostate &) { return "#eeeeee"; },
                                                [](const Core::AssignInstruction &) { return "#e8f5e9"; },
                                                [](const Core::CallInstruction &) { return "#e3f2fd"; },
                                                [](const Core::ReturnInstruction &) { return "#e1bee7"; },
                                                [](const Core::CondInstruction &) { return "#fff8e1"; },
                                                [](const Core::SwitchInstruction &) { return "#fff3e0"; },
                                                [](const Core::GotoInstruction &) { return "#f3e5f5"; },
                                                [](const Core::PhiInstruction &) { return "#fce4ec"; },
                                                [](const Core::AbortInstruction &) { return "#ef9a9a"; },
                                                [](const Core::LabelInstruction &) { return "#cfd8dc"; },
                                                [](const Core::AsmInstruction &) { return "#d7ccc8"; },
                                                [](const Core::ClobberInstruction &) { return "#ffebee"; },
                                                [](const Core::UnreachableInstruction &) { return "#bcaaa4"; },
                                                [](const Core::UnknownInstruction &) { return "#eeeeee"; }},
                                     instr.data);

    std::string readable_expr = formatInstructionText(model, instr);

    std::string tooltip_attr = "";
    if (!instr.source_location.file.empty() && instr.source_location.file != "<unknown>")
    {
        // TODO: let user click to open source file?
        tooltip_attr = " tooltip=\"" + escape(instr.source_location.file) + ":" +
                       std::to_string(instr.source_location.line) + "\" href=\"#\"";
    }

    ss << "                <tr>\n"
       << "                    <td bgcolor=\"" << bgcolor << "\" align=\"right\" width=\"30\"" << tooltip_attr << ">"
       << instr.id << "</td>\n"
       << "                    <td bgcolor=\"" << bgcolor << "\" align=\"left\"" << tooltip_attr << ">" << readable_expr
       << "</td>\n"
       << "                </tr>\n";

    return ss.str();
}

std::string DOTExporter::formatOperand(const Core::CodeModel &model, const Core::Operand &op)
{
    return std::visit(overloaded{[&](const Core::ConstantOperand &co) { return escape(co.value); },
                                 [&](const Core::VariableOperand &vo) {
                                     const auto *var = model.getVariable(vo.id);
                                     std::string res = var ? escape(var->name) : "???";
                                     for (const auto &acc : vo.access_path)
                                     {
                                         res = formatAccessor(model, acc, res);
                                     }
                                     return res;
                                 }},
                      op);
}

std::string DOTExporter::formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc,
                                        const std::string &base)
{
    return std::visit(
        overloaded{
            [&](const Core::DerefAccessor &) { return "*(" + base + ")"; },
            [&](const Core::ArrayAccessor &a) { return base + "[" + formatOperand(model, a.index) + "]"; },
            [&](const Core::FieldAccessor &f) {
                const auto *var = model.getVariable(f.field_id);
                return base + "." + (var ? escape(var->name) : "???");
            },
            [&](const Core::AddressOfAccessor &) { return "&amp;(" + base + ")"; },
            [&](const Core::OffsetAccessor &o) { return "(" + base + " + " + formatOperand(model, o.offset) + ")"; },
            [&](const Core::BitSliceAccessor &b) {
                return base + "[" + std::to_string(b.bit_start) + ":" + std::to_string(b.bit_start + b.bit_size) + "]";
            }},
        acc.data);
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
    case Core::OpCode::NEGATE:
        return "-";
    case Core::OpCode::BIT_NOT:
        return "~";
    case Core::OpCode::LOG_NOT:
        return "!";
    case Core::OpCode::ABS:
        return "ABS";

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

    case Core::OpCode::POINTER_ADD:
        return "+";
    case Core::OpCode::POINTER_SUB:
        return "-";

    case Core::OpCode::MIN:
        return "MIN";
    case Core::OpCode::MAX:
        return "MAX";

    case Core::OpCode::ROTATE_LEFT:
        return "ROL";
    case Core::OpCode::ROTATE_RIGHT:
        return "ROR";

    case Core::OpCode::CAST:
        return "CAST";

    default:
        return "";
    }
}

std::string DOTExporter::formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr)
{
    // helper to extract just the raw type name string
    auto getRawTypeName = [&](const Core::Operand &op) -> std::string {
        if (auto *var_op = std::get_if<Core::VariableOperand>(&op))
        {
            if (const auto *var = model.getVariable(var_op->id))
            {
                if (const auto *type = model.getType(var->type_id))
                {
                    return type->name;
                }
            }
        }
        return "";
    };

    // helper to format the type for the left-hand side assignment visualization
    auto getLhsTypeHTML = [&](const Core::Operand &op) -> std::string {
        std::string name = getRawTypeName(op);
        if (!name.empty())
        {
            return "<font color=\"#2e7d32\">" + escape(name) + "</font> ";
        }
        return "";
    };

    return std::visit(
        overloaded{
            [&](const std::monostate &) -> std::string { return "<b>&lt;empty instruction&gt;</b>"; },
            [&](const Core::AssignInstruction &assign) -> std::string {
                std::string type_html = getLhsTypeHTML(assign.lhs);
                std::string res = "<b>" + type_html + formatOperand(model, assign.lhs) + "</b> = ";

                if (assign.opcode == Core::OpCode::CAST && assign.rhs1)
                {
                    std::string cast_type = getRawTypeName(assign.lhs);
                    std::string cast_str = cast_type.empty() ? "cast" : escape(cast_type);
                    res += "(" + cast_str + ") " + formatOperand(model, *assign.rhs1);
                }
                else if (assign.opcode == Core::OpCode::MAX || assign.opcode == Core::OpCode::MIN ||
                         assign.opcode == Core::OpCode::ABS || assign.opcode == Core::OpCode::ROTATE_LEFT ||
                         assign.opcode == Core::OpCode::ROTATE_RIGHT)
                {
                    // format as a function call (e.g. MAX(a, b))
                    res += "<b>" + opCodeToString(assign.opcode) + "</b>(" + formatOperand(model, *assign.rhs1);
                    if (assign.rhs2)
                        res += ", " + formatOperand(model, *assign.rhs2);
                    res += ")";
                }
                else if (assign.opcode != Core::OpCode::NONE)
                {
                    // format as a standard infix/prefix operator (e.g. a + b)
                    std::string op_str = opCodeToString(assign.opcode);
                    if (assign.rhs2)
                    {
                        res += formatOperand(model, *assign.rhs1) + " " + op_str + " " +
                               formatOperand(model, *assign.rhs2);
                    }
                    else if (assign.rhs1)
                    {
                        res += op_str + " " + formatOperand(model, *assign.rhs1);
                    }
                }
                else if (assign.rhs1)
                {
                    res += formatOperand(model, *assign.rhs1);
                }
                return res;
            },
            [&](const Core::CallInstruction &call) -> std::string {
                std::string res = "";
                if (call.lhs)
                {
                    res += "<b>" + getLhsTypeHTML(*call.lhs) + formatOperand(model, *call.lhs) + "</b> = ";
                }
                res += "<b>call</b> " + formatOperand(model, call.callee) + "(";
                for (size_t i = 0; i < call.arguments.size(); ++i)
                {
                    res += formatOperand(model, call.arguments[i]);
                    if (i + 1 < call.arguments.size())
                        res += ", ";
                }
                res += ")";
                return res;
            },
            [&](const Core::CondInstruction &cond) -> std::string {
                return "<b>if</b> (" + formatOperand(model, cond.lhs) + " " + opCodeToString(cond.opcode) + " " +
                       formatOperand(model, cond.rhs) + ")";
            },
            [&](const Core::ReturnInstruction &ret) -> std::string {
                return "<b>return</b> " + (ret.return_value ? formatOperand(model, *ret.return_value) : "");
            },
            [&](const Core::SwitchInstruction &sw) -> std::string {
                return "<b>switch</b> (" + formatOperand(model, sw.index) + ")";
            },
            [&](const Core::GotoInstruction &gt) -> std::string {
                const auto *target_block = model.getBlock(gt.target);
                std::string block_name = target_block ? target_block->name : "unknown";

                return "<b>goto</b> " + escape(block_name);
            },
            [&](const Core::LabelInstruction &lbl) -> std::string {
                return "<b>" + formatOperand(model, lbl.label) + ":</b>";
            },
            [&](const Core::ClobberInstruction &cl) -> std::string {
                return "<b><font color=\"#c62828\">clobber</font></b> " + formatOperand(model, cl.clobbered_variable);
            },
            [&](const Core::PhiInstruction &phi) -> std::string {
                std::string res =
                    "<b>" + getLhsTypeHTML(phi.lhs) + formatOperand(model, phi.lhs) + "</b> = <b>phi</b>(";
                for (size_t i = 0; i < phi.incoming_values.size(); ++i)
                {
                    const auto *src_block = model.getBlock(phi.incoming_values[i].block_id);
                    std::string block_name = src_block ? src_block->name : "unknown";

                    res += "[" + escape(block_name) + ": " + formatOperand(model, phi.incoming_values[i].value) + "]";

                    if (i + 1 < phi.incoming_values.size())
                    {
                        res += ", ";
                    }
                }
                res += ")";
                return res;
            },
            [&](const Core::AbortInstruction &) -> std::string { return "<b>abort()</b>"; },
            [&](const Core::UnreachableInstruction &) -> std::string { return "<b>unreachable</b>"; },
            [&](const Core::AsmInstruction &) -> std::string { return "<b>asm(NOT HANDLED BY THE ADAPTER YET)</b>"; },
            [&](const Core::UnknownInstruction &u) -> std::string {
                return "<b>unknown</b>: " + escape(u.description);
            }},
        instr.data);
}

} // namespace CodeListener::Exporters
