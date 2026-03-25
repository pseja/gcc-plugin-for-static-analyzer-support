#pragma once

#include <optional>

#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::AnnotationServices
{

struct CallGraphEdge
{
    Core::InstructionId call_instruction;
    // target function ID
    // - if nullopt, the call is an indirect call (function pointer)
    std::optional<Core::FunctionId> callee;
};

} // namespace CodeListener::AnnotationServices
