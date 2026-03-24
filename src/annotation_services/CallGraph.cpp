#include <variant>

#include "AnalysisKey.hpp"
#include "CallGraph.hpp"
#include "CallInstruction.hpp"
#include "Operand.hpp"
#include "Variable.hpp"

namespace CodeListener::AnnotationServices
{

AnalysisKey CallGraph::Key;

CallGraph CallGraph::build(const Core::CodeModel &model)
{
    CallGraph graph;

    std::unordered_map<std::string, Core::FunctionId> function_name_to_id;
    for (const auto &function : model.getFunctions())
    {
        function_name_to_id[function.name] = function.id;
    }

    for (const auto &function : model.getFunctions())
    {
        for (auto block_id : function.block_ids)
        {
            const auto *block = model.getBlock(block_id);
            for (auto instruction_id : block->instruction_ids)
            {
                const auto *instruction = model.getInstruction(instruction_id);
                auto *call = std::get_if<Core::CallInstruction>(&instruction->data);
                if (!call)
                {
                    continue;
                }

                auto *var_op = std::get_if<Core::VariableOperand>(&call->callee);
                if (!var_op)
                {
                    continue;
                }

                const auto *var = model.getVariable(var_op->id);
                if (!var)
                {
                    continue;
                }

                auto it = function_name_to_id.find(var->name);
                if (it != function_name_to_id.end())
                {
                    graph.calls[function.id].push_back(it->second);
                    graph.called_by[it->second].push_back(function.id);
                }
            }
        }
    }

    return graph;
}

} // namespace CodeListener::AnnotationServices
