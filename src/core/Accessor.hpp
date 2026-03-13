#pragma once

#include "AccessorKind.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

struct Accessor
{
    AccessorKind kind;
    // if FIELD/COMPONENT: the id of the field declaration (variable)
    VariableId target_field_variable_id;
    TypeId target_field_type_id;
    // if ARRAY/OFFSET: the index can be a constant (string) or a variable (NodeId)
    VariableId index_operand_id;

    // for BIT_SLICE
    unsigned bit_start = 0;
    unsigned bit_size = 0;
};

} // namespace CodeListener::Core
