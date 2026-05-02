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
    FunctionId parent;
    std::string name;

    // ast children
    std::vector<InstructionId> instruction_ids;

    // cfg edges
    std::vector<BlockId> predecessors;
    std::vector<BlockId> successors;
};

} // namespace CodeListener::Core
