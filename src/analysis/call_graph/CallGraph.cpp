/**
 * @file CallGraph.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements call graph construction from the CodeModel.
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

#include <algorithm>
#include <stack>
#include <unordered_set>
#include <variant>

#include "AssignInstruction.hpp"
#include "CallGraph.hpp"
#include "CallInstruction.hpp"
#include "Instruction.hpp"
#include "InstructionId.hpp"
#include "Operand.hpp"
#include "ReturnInstruction.hpp"
#include "Variable.hpp"

namespace CodeListener::AnnotationServices
{

CallGraph CallGraph::build(const Core::CodeModel &model)
{
    CallGraph graph;

    buildGraph(graph, model);
    computeRootsAndLeaves(graph);
    computeTopologicalOrder(graph);

    return graph;
}

void CallGraph::buildGraph(CallGraph &graph, const Core::CodeModel &model)
{
    std::unordered_map<std::string, Core::FunctionId> function_name_to_id;
    for (const Core::Function &function : model.functions())
    {
        function_name_to_id[function.name] = function.id;
        graph.nodes[function.id].function_id = function.id;
    }

    auto record_address_taken = [&](const Core::Operand &op, Core::InstructionId instr_id) {
        if (const Core::VariableOperand *var_op = std::get_if<Core::VariableOperand>(&op))
        {
            if (const Core::Variable *var = model.getVariable(var_op->id))
            {
                auto it = function_name_to_id.find(var->name);
                if (it != function_name_to_id.end())
                {
                    graph.nodes[it->second].address_taken_at.push_back(instr_id);
                    graph.has_callbacks = true;
                }
            }
        }
    };

    for (const Core::Function &function : model.functions())
    {
        Core::FunctionId caller_id = function.id;

        for (Core::BlockId block_id : function.block_ids)
        {
            const Core::Block *block = model.getBlock(block_id);
            if (!block)
            {
                continue;
            }

            for (Core::InstructionId instruction_id : block->instruction_ids)
            {
                const Core::Instruction *instruction = model.getInstruction(instruction_id);
                if (!instruction)
                {
                    continue;
                }

                // check for CallInstruction
                if (const Core::CallInstruction *call = std::get_if<Core::CallInstruction>(&instruction->data))
                {
                    std::optional<Core::FunctionId> callee_id;
                    bool is_indirect = true;

                    // try to resolve direct call
                    if (const Core::VariableOperand *var_op = std::get_if<Core::VariableOperand>(&call->callee))
                    {
                        if (const Core::Variable *var = model.getVariable(var_op->id))
                        {
                            auto it = function_name_to_id.find(var->name);
                            if (it != function_name_to_id.end())
                            {
                                callee_id = it->second;
                                is_indirect = false;
                            }
                        }
                    }

                    CallGraphEdge edge{instruction_id, callee_id};
                    graph.nodes[caller_id].outgoing_calls.push_back(edge);

                    if (!is_indirect)
                    {
                        graph.nodes[callee_id.value()].incoming_calls.push_back(edge);
                    }
                    else
                    {
                        graph.has_indirect_calls = true;
                    }

                    // check arguments for function pointers (callbacks)
                    for (const Core::Operand &arg : call->arguments)
                    {
                        record_address_taken(arg, instruction_id);
                    }
                }
                // address taken in assignments
                else if (const Core::AssignInstruction *assign =
                             std::get_if<Core::AssignInstruction>(&instruction->data))
                {
                    if (assign->rhs1.has_value())
                    {
                        record_address_taken(assign->rhs1.value(), instruction_id);
                    }
                    if (assign->rhs2.has_value())
                    {
                        record_address_taken(assign->rhs2.value(), instruction_id);
                    }
                    if (assign->rhs3.has_value())
                    {
                        record_address_taken(assign->rhs3.value(), instruction_id);
                    }
                }
                // address taken in returns
                else if (const Core::ReturnInstruction *ret = std::get_if<Core::ReturnInstruction>(&instruction->data))
                {
                    if (ret->return_value.has_value())
                    {
                        record_address_taken(ret->return_value.value(), instruction_id);
                    }
                }
            }
        }
    }
}

void CallGraph::computeRootsAndLeaves(CallGraph &graph)
{
    for (const auto &[function_id, function] : graph.nodes)
    {
        if (function.incoming_calls.empty())
        {
            graph.roots.push_back(function_id);
        }
        if (function.outgoing_calls.empty())
        {
            graph.leaves.push_back(function_id);
        }
    }
}

void CallGraph::computeTopologicalOrder(CallGraph &graph)
{
    struct TarjanState
    {
        int index{0};
        std::stack<Core::FunctionId> stack;
        std::unordered_map<Core::FunctionId, int> indices;
        std::unordered_map<Core::FunctionId, int> lowlinks;
        std::unordered_set<Core::FunctionId> on_stack;
    } state;

    auto strongconnect = [&](auto self, Core::FunctionId v) -> void {
        state.indices[v] = state.index;
        state.lowlinks[v] = state.index;
        state.index++;
        state.stack.push(v);
        state.on_stack.insert(v);

        if (graph.nodes.count(v))
        {
            for (const auto &edge : graph.nodes.at(v).outgoing_calls)
            {
                if (!edge.callee.has_value())
                {
                    continue;
                }

                Core::FunctionId w = edge.callee.value();
                if (state.indices.find(w) == state.indices.end())
                {
                    self(self, w);
                    state.lowlinks[v] = std::min(state.lowlinks[v], state.lowlinks[w]);
                }
                else if (state.on_stack.count(w))
                {
                    state.lowlinks[v] = std::min(state.lowlinks[v], state.indices[w]);
                }
            }
        }

        if (state.lowlinks[v] == state.indices[v])
        {
            Core::FunctionId w;
            std::vector<Core::FunctionId> scc;

            do
            {
                w = state.stack.top();
                state.stack.pop();
                state.on_stack.erase(w);
                scc.push_back(w);
            } while (w != v);

            graph.topological_order.push_back(scc);
        }
    };

    for (const auto &[function_id, function] : graph.nodes)
    {
        if (state.indices.find(function_id) == state.indices.end())
        {
            strongconnect(strongconnect, function_id);
        }
    }

    // Tarjan's algorithm returns SCCs in reverse topological order
    std::reverse(graph.topological_order.begin(), graph.topological_order.end());
}

} // namespace CodeListener::AnnotationServices
