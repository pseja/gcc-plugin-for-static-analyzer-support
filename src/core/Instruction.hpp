#pragma once

#include <string>
#include <vector>

#include "InstructionKind.hpp"
#include "NodeId.hpp"
#include "OpCode.hpp"
#include "Operand.hpp"
#include "SourceLocation.hpp"

namespace CodeListener
{

namespace Core
{

struct Instruction
{
    NodeId id;
    NodeId parent_block_id;

    InstructionKind kind;
    OpCode opcode = OpCode::NONE;
    std::string opcode_name;

    std::vector<Operand> operands;
    SourceLocation source_location;

    // ends a basic block (e.g. GOTO, RET, SWITCH, COND)
    bool is_terminator = false;
};

} // namespace Core

} // namespace CodeListener
