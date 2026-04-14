#pragma once

#include <string>

#include "Operand.hpp"

namespace CodeListener::Core
{

struct AsmOperandConstraint
{
    // "=r" (write to any register), "r" (read from any register), "m" (memory operand)
    std::string constraint;
    Operand operand;
};

} // namespace CodeListener::Core
