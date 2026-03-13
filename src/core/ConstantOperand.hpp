#pragma once

#include <string>

#include "VariableId.hpp"

namespace CodeListener::Core
{

struct ConstantOperand
{
    VariableId id;
    std::string value;
};

} // namespace CodeListener::Core
