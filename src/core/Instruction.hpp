#pragma once

#include <string>
#include <vector>

#include "NodeId.hpp"
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
    // TODO: map standard GCC codes (GIMPLE_ASSIGN, ...) to this
    std::string opcode;
    std::vector<Operand> operands;
    SourceLocation source_location;
};

} // namespace Core

} // namespace CodeListener
