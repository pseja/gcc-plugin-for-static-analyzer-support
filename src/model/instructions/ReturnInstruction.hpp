#pragma once

#include <optional>

#include "Operand.hpp"

namespace CodeListener::Core
{

struct ReturnInstruction
{
    std::optional<Operand> return_value; // std::nullopt for void returns
};

} // namespace CodeListener::Core
