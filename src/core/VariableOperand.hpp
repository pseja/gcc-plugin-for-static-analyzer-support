#pragma once

#include <vector>

#include "Accessor.hpp"
#include "NodeId.hpp"

namespace CodeListener
{

namespace Core
{

struct VariableOperand
{
    NodeId variable_id;

    std::vector<Accessor> access_path;
};

} // namespace Core

} // namespace CodeListener
