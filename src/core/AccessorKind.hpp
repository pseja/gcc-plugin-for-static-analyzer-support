#pragma once

#include <string>

namespace CodeListener
{

namespace Core
{

enum class AccessorKind
{
    DEREF,      // *ptr
    ARRAY,      // ptr[index] or arr[index]
    FIELD,      // obj.field or ptr->field
    ADDRESS_OF, // &expr
    OFFSET,     // (char*)ptr + offset (pointer arithmetic)
    BIT_SLICE,  // expr[start:end] (bit field extraction)
};

inline std::string toString(AccessorKind accessor_kind)
{
    switch (accessor_kind)
    {
    case AccessorKind::DEREF:
        return "DEREF";
    case AccessorKind::ARRAY:
        return "ARRAY";
    case AccessorKind::FIELD:
        return "FIELD";
    case AccessorKind::ADDRESS_OF:
        return "ADDRESS_OF";
    case AccessorKind::OFFSET:
        return "OFFSET";
    case AccessorKind::BIT_SLICE:
        return "BIT_SLICE";
    default:
        return "UNKNOWN";
    }
}

} // namespace Core

} // namespace CodeListener
