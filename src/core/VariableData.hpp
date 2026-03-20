#pragma once

#include <variant>

#include "FieldVariable.hpp"
#include "StandardVariable.hpp"

namespace CodeListener::Core
{

using VariableData = std::variant<StandardVariable, FieldVariable>;

} // namespace CodeListener::Core
