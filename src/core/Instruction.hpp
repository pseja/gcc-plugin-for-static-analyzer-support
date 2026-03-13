#pragma once

#include <string>
#include <vector>

#include "BlockId.hpp"
#include "InstructionId.hpp"
#include "InstructionKind.hpp"
#include "OpCode.hpp"
#include "SourceLocation.hpp"
#include "SwitchCase.hpp"

namespace CodeListener::Core
{

struct Instruction
{
    InstructionId id;
    BlockId parent_block_id;

    InstructionKind kind;
    OpCode opcode = OpCode::NONE;
    std::string opcode_name;

    // TODO: replace these with specific fields for different instruction kinds (e.g. CallInstruction with callee and
    // arguments)
    std::vector<Operand> operands;
    std::vector<SwitchCase> switch_cases;

    SourceLocation source_location;

    // ends a basic block (e.g. GOTO, RET, SWITCH, COND)
    bool is_terminator = false;
};

} // namespace CodeListener::Core
