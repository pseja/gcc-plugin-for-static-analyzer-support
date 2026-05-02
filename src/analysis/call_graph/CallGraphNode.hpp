#pragma once

#include <vector>

#include "CallGraphEdge.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::AnnotationServices
{

// function in the call graph
struct CallGraphNode
{
    Core::FunctionId function_id;

    std::vector<CallGraphEdge> outgoing_calls;
    std::vector<CallGraphEdge> incoming_calls;

    // instructions where the address of this function was taken
    std::vector<Core::InstructionId> address_taken_at;
};

} // namespace CodeListener::AnnotationServices
