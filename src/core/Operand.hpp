#pragma once

#include <variant>
#include <vector>

#include "AccessorKind.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

struct Accessor;

struct ConstantOperand
{
    TypeId type_id;
    std::string value;
};

struct VariableOperand
{
    VariableId id;
    std::vector<Accessor> access_path;
    std::optional<TypeId> result_type_id; // type of the full expression, if known
};

using Operand = std::variant<ConstantOperand, VariableOperand>;

struct DerefAccessor
{
    // *ptr
};

struct ArrayAccessor
{
    // ptr[index] or arr[index]
    Operand index;
};

struct FieldAccessor
{
    // obj.field or ptr->field
    VariableId field_id;
};

struct AddressOfAccessor
{
    // &expr
    TypeId target_type_id;
};

struct OffsetAccessor
{
    // (char*)ptr + offset (pointer arithmetic)
    Operand offset;
};

struct BitSliceAccessor
{
    // expr[start:end] (bit field extraction)
    unsigned bit_start;
    unsigned bit_size;
};

using AccessorData =
    std::variant<DerefAccessor, ArrayAccessor, FieldAccessor, AddressOfAccessor, OffsetAccessor, BitSliceAccessor>;

struct Accessor
{
    // FIXME: not needed anymore thanks to the AccessorData variant, but keeping it for easier querying and debugging
    // for now
    AccessorKind kind;
    AccessorData data;
};

} // namespace CodeListener::Core
