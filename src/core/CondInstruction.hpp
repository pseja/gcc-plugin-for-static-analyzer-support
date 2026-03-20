#pragma once

#include "BlockId.hpp"
#include "OpCode.hpp"
#include "Operand.hpp"

namespace CodeListener::Core
{

struct CondInstruction
{
    Operand lhs;
    Operand rhs;
    OpCode opcode;

    BlockId true_target;
    BlockId false_target;
};

} // namespace CodeListener::Core
