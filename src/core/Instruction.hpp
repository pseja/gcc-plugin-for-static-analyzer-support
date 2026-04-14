#pragma once

#include "BlockId.hpp"
#include "InstructionData.hpp"
#include "InstructionId.hpp"
#include "InstructionKind.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

struct Instruction
{
    InstructionId id;
    BlockId parent_block_id;

    // FIXME: not needed anymore thanks to the InstructionData variant, but keeping it for easier querying and debugging
    // for now
    InstructionKind kind;
    InstructionData data;

    SourceLocation source_location;

    // ends a basic block (e.g. GOTO, RET, SWITCH, COND)
    bool is_terminator{false};
};

} // namespace CodeListener::Core
