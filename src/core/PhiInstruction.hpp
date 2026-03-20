#pragma once

#include "Operand.hpp"
#include "PhiIncomingValue.hpp"

namespace CodeListener::Core
{

struct PhiInstruction
{
    Operand lhs;
    std::vector<PhiIncomingValue> incoming_values;
};

} // namespace CodeListener::Core
