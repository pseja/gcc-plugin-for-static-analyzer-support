#pragma once

#include <variant>

#include "ConstantOperand.hpp"
#include "VariableOperand.hpp"

namespace CodeListener::Core
{

using Operand = std::variant<VariableOperand, ConstantOperand>;

} // namespace CodeListener::Core
