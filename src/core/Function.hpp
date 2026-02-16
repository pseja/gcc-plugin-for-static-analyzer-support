#pragma once

#include <string>
#include <vector>

#include "NodeId.hpp"

namespace CodeListener
{

namespace Core
{

struct Function
{
    NodeId id;
    std::string name;
    NodeId return_type_id;

    std::vector<NodeId> parameter_ids;

    std::vector<NodeId> local_variable_ids;

    // ast children (blocks)
    std::vector<NodeId> block_ids;
};

} // namespace Core

} // namespace CodeListener
