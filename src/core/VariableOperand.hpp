#pragma once

#include <vector>

#include "Accessor.hpp"
#include "NodeId.hpp"

namespace CodeListener::Core
{

struct VariableOperand
{
    NodeId variable_id;

    std::vector<Accessor> access_path;
};

} // namespace CodeListener::Core
