#pragma once

#include "NodeId.hpp"
#include "AccessorKind.hpp"

namespace CodeListener::Core
{

struct Accessor
{
    AccessorKind kind;
    // if FIELD/COMPONENT: the id of the field declaration (variable)
    NodeId target_field_id = NodeId::INVALID;
    // if ARRAY/OFFSET: the index can be a constant (string) or a variable (NodeId)
    NodeId index_operand_id = NodeId::INVALID;

    // for BIT_SLICE
    unsigned bit_start = 0;
    unsigned bit_size = 0;
};

} // namespace CodeListener::Core
