#pragma once

#include <vector>

#include "VariableId.hpp"

namespace CodeListener::Core
{

struct StructType
{
    std::vector<VariableId> fields;
};

} // namespace CodeListener::Core
