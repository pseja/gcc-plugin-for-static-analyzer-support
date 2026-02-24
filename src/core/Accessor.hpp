#pragma once

#include "NodeId.hpp"
#include "AccessorKind.hpp"

namespace CodeListener
{

namespace Core
{

struct Accessor
{
    AccessorKind kind;
    // if FIELD/COMPONENT: the id of the field declaration (variable)
    NodeId target_field_id = NodeId::INVALID;
    // if ARRAY: the index can be a constant (string) or a variable (NodeId)
    NodeId index_operand_id = NodeId::INVALID;
};

} // namespace Core

} // namespace CodeListener
