#pragma once

#include <vector>

#include "VariableId.hpp"

namespace CodeListener::Core
{

struct UnionType
{
    std::vector<VariableId> fields;
};

} // namespace CodeListener::Core
