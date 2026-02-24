#pragma once

#include <vector>
#include <variant>

#include "Accessor.hpp"
#include "AccessorKind.hpp"
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
