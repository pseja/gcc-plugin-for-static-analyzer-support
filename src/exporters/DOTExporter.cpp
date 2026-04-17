#include <sstream>

#include "CallGraph.hpp"
#include "DOTExporter.hpp"
#include "OpCode.hpp"
#include "utility.hpp"

namespace CodeListener::Exporters
{

DOTExporter::DOTExporter(std::ostream &os, DotVerbosity verbosity) : os(os), verbosity(verbosity)
{
}

DOTExporter::DOTExporter(const std::string &filepath, DotVerbosity verbosity)
    : file_os(filepath), os(file_os), verbosity(verbosity)
{
}

// Returns true when 'instr' is an explicit CFG terminal (goto / if / ret / switch / abort).
static bool isCleanTerminal(const Core::Instruction &instr)
{
    return std::visit(
        overloaded{
            [](const Core::ReturnInstruction &) { return true; }, [](const Core::GotoInstruction &) { return true; },
            [](const Core::CondInstruction &) { return true; }, [](const Core::SwitchInstruction &) { return true; },
            [](const Core::AbortInstruction &) { return true; }, [](const auto &) { return false; }},
        instr.data);
}

// Returns true if the block in CLEAN mode needs a "..." body node.
static bool cleanHasBody(const Core::CodeModel &model, const Core::Block &block)
{
    if (block.instruction_ids.empty())
    {
        return false;
    }

    const auto *last = model.getInstruction(block.instruction_ids.back());
    if (!last)
    {
        return false;
    }

    return isCleanTerminal(*last) ? (block.instruction_ids.size() > 1) : true;
}

bool DOTExporter::shouldVisitBlock(const Core::CodeModel &, const Core::Block &block)
{
    if (verbosity == DotVerbosity::FULL)
    {
        return true;
    }
    if (block.name == "ENTRY" || block.name == "EXIT")
    {
        return false;
    }
    if (verbosity == DotVerbosity::COMPACT && block.instruction_ids.empty())
    {
        return false;
    }

    return true;
}

void DOTExporter::onBeginModel(const Core::CodeModel &)
{
    edge_buffer.str("");
    edge_buffer.clear();
    curr_file.clear();
    file_cluster_id = 0;

    os << "digraph IR {\n";
    os << "    node [shape=none, fontname=\"Courier New\", fontsize=10];\n";
    os << "    edge [fontname=\"Courier New\", fontsize=9];\n";
    os << "    graph [rankdir=TB, splines=polyline, compound=true];\n\n";
}

void DOTExporter::closeFileCluster()
{
    if (!curr_file.empty())
    {
        os << "    }\n\n";
        curr_file.clear();
    }
}

void DOTExporter::openFileCluster(const std::string &file)
{
    closeFileCluster();

    curr_file = file;
    os << "    subgraph cluster_file_" << file_cluster_id++ << " {\n";
    os << "        label=" << '"' << escape(file) << '"' << ";\n";
    os << "        color=\"#d1d5db\";\n";
    os << "        style=dashed;\n";
    os << "        bgcolor=\"#f9fafb\";\n";
    os << "        margin=20;\n\n";
}

void DOTExporter::onEndModel(const Core::CodeModel &model)
{
    closeFileCluster();
    const auto &cg = analysis_manager.getAnnotation<AnnotationServices::CallGraph>(model);

    for (const auto &[caller_id, node] : cg.nodes)
    {
        for (const auto &edge : node.outgoing_calls)
        {
            if (!edge.callee)
            {
                continue;
            }

            auto callee_id = *edge.callee;
            const auto *callee = model.getFunction(callee_id);
            if (!callee || callee->block_ids.empty())
            {
                continue;
            }

            const auto *instr = model.getInstruction(edge.call_instruction);
            if (!instr)
            {
                continue;
            }

            const Core::Block *target_block = nullptr;
            for (const auto &bid : callee->block_ids)
            {
                if (isBlockVisible(model, bid))
                {
                    target_block = model.getBlock(bid);
                    break;
                }
            }
            if (!target_block)
            {
                continue;
            }

            if (verbosity == DotVerbosity::CLEAN)
            {
                const auto *src_block = model.getBlock(instr->parent_block_id);
                bool has_body = src_block && cleanHasBody(model, *src_block);
                os << "    block_" << instr->parent_block_id << (has_body ? "_1" : "_0") << " -> block_"
                   << target_block->id << "_0"
                   << " [lhead=cluster_func_" << callee_id << ", color=\"#6c757d\"];\n";
            }
            else
            {
                os << "    block_" << instr->parent_block_id << " -> block_" << target_block->id
                   << " [lhead=cluster_func_" << callee_id << ", color=\"#6c757d\"];\n";
            }
        }
    }

    os << edge_buffer.str();

    os << "}\n";
}

void DOTExporter::onBeginFunction(const Core::CodeModel &, const Core::Function &func)
{
    const std::string &file = func.source_location.file;
    if (file != curr_file)
    {
        openFileCluster(file);
    }

    os << "        subgraph cluster_func_" << func.id << " {\n";
    os << "            label=<<b>" << escape(func.name) << "()</b>>;\n";
    os << "            style=filled;\n";
    os << "            fillcolor=\"#ffffff\";\n";
    os << "            color=\"#9ca3af\";\n";
    os << "            margin=15;\n\n";
}

void DOTExporter::onEndFunction(const Core::CodeModel &, const Core::Function &)
{
    os << "        }\n\n";
}

void DOTExporter::onBeginBlock(const Core::CodeModel &model, const Core::Block &block)
{
    if (verbosity == DotVerbosity::CLEAN)
    {
        os << "        subgraph cluster_block_" << block.id << " {\n";
        os << "            label=\"" << escape(block.name) << "\";\n";
        os << "            color=\"#6b7280\";\n";
        os << "            style=dashed;\n";
        os << "            bgcolor=\"#ffffff\";\n";
        os << "            margin=8;\n";

        if (block.instruction_ids.empty())
        {
            os << "            block_" << block.id << "_0 [shape=box, color=\"#9ca3af\", "
               << "fontcolor=\"#9ca3af\", style=dotted, label=\"(empty)\"];\n";
        }
        else
        {
            const auto *last_instr = model.getInstruction(block.instruction_ids.back());
            bool has_body = cleanHasBody(model, block);
            if (has_body)
            {
                os << "            block_" << block.id << "_0 [shape=box, color=\"#9ca3af\", "
                   << "fontcolor=\"#9ca3af\", style=dotted, label=\"...\"];\n";
                os << "            block_" << block.id << "_0 -> block_" << block.id << "_1 "
                   << "[color=\"#9ca3af\", style=dotted, arrowhead=open];\n";
            }

            const std::string term_suffix = has_body ? "_1" : "_0";

            auto [term_label, term_color, term_style] = DOTExporter::cleanTerminalInfo(*last_instr);
            os << "            block_" << block.id << term_suffix << " [shape=box, color=\"" << term_color
               << "\", fontcolor=\"" << term_color << "\", style=" << term_style << ", label=\"" << term_label
               << "\"];\n";
        }

        os << "        }\n";
        return;
    }

    os << "        block_" << block.id << " [label=<\n";
    os << "            <table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n";

    if (verbosity == DotVerbosity::FULL)
    {
        os << "                <tr><td bgcolor=\"#f3f4f6\" colspan=\"2\" align=\"center\">"
           << "<b>Block " << escape(block.name) << " (" << block.id << ")</b></td></tr>\n";
    }
    else
    {
        os << "                <tr><td bgcolor=\"#f3f4f6\" align=\"center\">"
           << "<b>" << escape(block.name) << "</b></td></tr>\n";
    }

    if (block.instruction_ids.empty())
    {
        const char *span = (verbosity == DotVerbosity::FULL) ? " colspan=\"2\"" : "";
        os << "                <tr><td bgcolor=\"#ffffff\"" << span << "><i>&lt;empty&gt;</i></td></tr>\n";
    }
}

void DOTExporter::onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    if (verbosity == DotVerbosity::CLEAN)
    {
        return;
    }

    os << exportInstruction(model, instr);
}

void DOTExporter::onEndBlock(const Core::CodeModel &model, const Core::Block &block)
{
    if (verbosity != DotVerbosity::CLEAN)
    {
        os << "            </table>\n";
        os << "        >];\n";
    }

    emitBlockEdges(model, block);
}

void DOTExporter::emitBlockEdges(const Core::CodeModel &model, const Core::Block &block)
{
    bool handled_edges = false;
    std::string src = edgeSrcNodeStr(model, block);

    if (!block.instruction_ids.empty())
    {
        const auto *last_instr = model.getInstruction(block.instruction_ids.back());

        if (auto *sw_instr = std::get_if<Core::SwitchInstruction>(&last_instr->data))
        {
            for (const auto &sw_case : sw_instr->cases)
            {
                if (sw_case.target_block_id.isValid() && isBlockVisible(model, sw_case.target_block_id))
                {
                    std::string label =
                        sw_case.low_value.has_value() ? formatOperand(model, sw_case.low_value.value()) : "default";
                    if (sw_case.high_value.has_value())
                    {
                        label += " ... " + formatOperand(model, sw_case.high_value.value());
                    }

                    edge_buffer << "    " << src << " -> " << edgeTargetNodeStr(sw_case.target_block_id) << " [label=\""
                                << escape(label) << "\", color=\"#d97706\", fontcolor=\"#d97706\"];\n";
                }
            }
            handled_edges = true;
        }
        else if (auto *cond_instr = std::get_if<Core::CondInstruction>(&last_instr->data))
        {
            if (cond_instr->true_target.isValid() && isBlockVisible(model, cond_instr->true_target))
            {
                edge_buffer << "    " << src << " -> " << edgeTargetNodeStr(cond_instr->true_target)
                            << " [label=\"true\", color=\"#059669\", fontcolor=\"#059669\"];\n";
            }
            if (cond_instr->false_target.isValid() && isBlockVisible(model, cond_instr->false_target))
            {
                edge_buffer << "    " << src << " -> " << edgeTargetNodeStr(cond_instr->false_target)
                            << " [label=\"false\", color=\"#dc2626\", fontcolor=\"#dc2626\"];\n";
            }
            handled_edges = true;
        }
    }

    if (!handled_edges)
    {
        for (auto succ_id : block.successors)
        {
            if (isBlockVisible(model, succ_id))
            {
                edge_buffer << "    " << src << " -> " << edgeTargetNodeStr(succ_id) << " [color=\"#6b7280\"];\n";
            }
        }
    }
}

bool DOTExporter::isBlockVisible(const Core::CodeModel &model, Core::BlockId id) const
{
    if (verbosity == DotVerbosity::FULL)
    {
        return true;
    }
    const auto *b = model.getBlock(id);
    if (!b)
    {
        return false;
    }
    if (b->name == "ENTRY" || b->name == "EXIT")
    {
        return false;
    }
    if (verbosity == DotVerbosity::COMPACT && b->instruction_ids.empty())
    {
        return false;
    }

    return true;
}

std::string DOTExporter::edgeSrcNodeStr(const Core::CodeModel &model, const Core::Block &block) const
{
    if (verbosity == DotVerbosity::CLEAN)
    {
        bool has_body = cleanHasBody(model, block);
        return "block_" + std::to_string(block.id.index) + (has_body ? "_1" : "_0");
    }

    return "block_" + std::to_string(block.id.index);
}

std::string DOTExporter::edgeTargetNodeStr(Core::BlockId id) const
{
    if (verbosity == DotVerbosity::CLEAN)
    {
        return "block_" + std::to_string(id.index) + "_0";
    }

    return "block_" + std::to_string(id.index);
}

std::tuple<const char *, const char *, const char *> DOTExporter::cleanTerminalInfo(const Core::Instruction &instr)
{
    if (!isCleanTerminal(instr))
    {
        return {"goto", "#6b7280", "bold"};
    }

    return std::visit(
        overloaded{[](const Core::ReturnInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"ret", "#7c3aed", "bold"};
                   },
                   [](const Core::GotoInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"goto", "#6b7280", "bold"};
                   },
                   [](const Core::CondInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"if", "#059669", "bold"};
                   },
                   [](const Core::SwitchInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"switch", "#d97706", "bold"};
                   },
                   [](const Core::CallInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"call", "#2563eb", "dashed"};
                   },
                   [](const Core::AbortInstruction &) -> std::tuple<const char *, const char *, const char *> {
                       return {"abort", "#dc2626", "bold"};
                   },
                   [](const auto &) -> std::tuple<const char *, const char *, const char *> {
                       return {"insn", "#9ca3af", "solid"};
                   }},
        instr.data);
}

std::string DOTExporter::exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr)
{
    std::ostringstream ss;

    std::string bgcolor = std::visit(overloaded{// standard execution
                                                [](const std::monostate &) { return "#ffffff"; },
                                                [](const Core::AssignInstruction &) { return "#ffffff"; },
                                                [](const Core::CondInstruction &) { return "#ffffff"; },
                                                [](const Core::PhiInstruction &) { return "#ffffff"; },
                                                [](const Core::LabelInstruction &) { return "#ffffff"; },
                                                [](const Core::UnknownInstruction &) { return "#ffffff"; },

                                                // inter-procedural
                                                [](const Core::CallInstruction &) { return "#eff6ff"; },

                                                // exiting
                                                [](const Core::ReturnInstruction &) { return "#f5f3ff"; },

                                                // danger/crash
                                                [](const Core::AbortInstruction &) { return "#fef2f2"; },
                                                [](const Core::ClobberInstruction &) { return "#fef2f2"; },
                                                [](const Core::UnreachableInstruction &) { return "#fef2f2"; },

                                                // branching
                                                [](const Core::SwitchInstruction &) { return "#fffbeb"; },
                                                [](const Core::GotoInstruction &) { return "#fffbeb"; },

                                                // hardware/direct memory
                                                [](const Core::AsmInstruction &) { return "#f1f5f9"; }},
                                     instr.data);

    std::string readable_expr = formatInstructionText(model, instr);

    if (verbosity == DotVerbosity::COMPACT)
    {
        ss << "                <tr>\n"
           << "                    <td bgcolor=\"" << bgcolor << "\" align=\"left\">" << readable_expr << "</td>\n"
           << "                </tr>\n";
        return ss.str();
    }

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
                                     std::string res;
                                     if (!var)
                                     {
                                         res = "???";
                                     }
                                     else if (!var->name.empty())
                                     {
                                         res = escape(var->name);
                                     }
                                     else if (var->artificial)
                                     {
                                         res = "<i>tmp." + std::to_string(vo.id.index) + "</i>";
                                     }
                                     else
                                     {
                                         res = "<i>anon." + std::to_string(vo.id.index) + "</i>";
                                     }
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
            return "<font color=\"#059669\">" + escape(name) + "</font> ";
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
                    {
                        res += ", ";
                    }
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
                return "<b><font color=\"#dc2626\">clobber</font></b> " + formatOperand(model, cl.clobbered_variable);
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
            [&](const Core::AsmInstruction &) -> std::string { return "<b>asm()</b>"; },
            [&](const Core::UnknownInstruction &u) -> std::string {
                return "<b>unknown</b>: " + escape(u.description);
            }},
        instr.data);
}

} // namespace CodeListener::Exporters
