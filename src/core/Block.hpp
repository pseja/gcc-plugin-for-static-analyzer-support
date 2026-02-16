#pragma once

#include <string> // std::string
#include <vector> // std::vector

#include "NodeId.hpp" // NodeId

namespace CodeListener
{

namespace Core
{

struct Block
{
    NodeId id;
    NodeId parent_function_id;
    std::string name;

    // ast children
    std::vector<NodeId> instruction_ids;

    // cfg edges
    std::vector<NodeId> predecesor_block_ids;
    std::vector<NodeId> successor_block_ids;
};

} // namespace Core

} // namespace CodeListener
