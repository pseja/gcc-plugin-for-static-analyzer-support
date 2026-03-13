#pragma once

#include <vector>

#include "Accessor.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

struct VariableOperand
{
    VariableId id;

    std::vector<Accessor> access_path;
};

} // namespace CodeListener::Core
