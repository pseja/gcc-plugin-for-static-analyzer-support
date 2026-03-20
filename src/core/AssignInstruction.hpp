#pragma once

#include <optional>

#include "OpCode.hpp"
#include "Operand.hpp"

namespace CodeListener::Core
{

struct AssignInstruction
{
    Operand lhs;
    OpCode opcode;

    std::optional<Operand> rhs1;
    std::optional<Operand> rhs2;
    std::optional<Operand> rhs3; // for ternary operators
};

} // namespace CodeListener::Core
