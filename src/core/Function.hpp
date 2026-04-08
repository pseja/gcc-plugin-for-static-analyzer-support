#pragma once

#include <string>
#include <vector>

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

struct Function
{
    FunctionId id;
    std::string name;
    TypeId return_type_id;
    SourceLocation source_location;

    std::vector<VariableId> parameter_ids;

    std::vector<VariableId> local_variable_ids;

    // ast children (blocks)
    std::vector<BlockId> block_ids;
};

} // namespace CodeListener::Core
