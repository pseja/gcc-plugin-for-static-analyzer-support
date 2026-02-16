#pragma once

// #include <vector>

#include "NodeId.hpp"

namespace CodeListener
{

namespace Core
{

struct VariableOperand
{
    NodeId variable_id;

    // TODO: x.field[0]
    // std::vector<int> access_offsets;
};

} // namespace Core

} // namespace CodeListener
