#pragma once

#include <string> // std::string
#include <vector> // std::vector

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::Core
{

struct Block
{
    BlockId id;
    FunctionId parent_function_id;
    std::string name;

    // ast children
    std::vector<InstructionId> instruction_ids;

    // cfg edges
    std::vector<BlockId> predecesor_block_ids;
    std::vector<BlockId> successor_block_ids;
};

} // namespace CodeListener::Core
