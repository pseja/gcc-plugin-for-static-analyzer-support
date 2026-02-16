#pragma once

#include <variant>

#include "ConstantOperand.hpp"
#include "VariableOperand.hpp"

namespace CodeListener
{

namespace Core
{

using Operand = std::variant<VariableOperand, ConstantOperand>;

} // namespace Core

} // namespace CodeListener
