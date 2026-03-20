#pragma once

#include <optional>
#include <vector>

#include "Operand.hpp"

namespace CodeListener::Core
{

struct CallInstruction
{
    std::optional<Operand> lhs; // std::nullopt for void functions
    Operand callee;
    std::vector<Operand> arguments;
};

} // namespace CodeListener::Core
