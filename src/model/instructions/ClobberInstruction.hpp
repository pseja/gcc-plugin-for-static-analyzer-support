#pragma once

#include "Operand.hpp"

namespace CodeListener::Core
{

struct ClobberInstruction
{
    Operand clobbered_variable;
};

} // namespace CodeListener::Core
