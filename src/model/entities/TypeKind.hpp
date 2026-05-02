#pragma once

#include <string_view>

namespace CodeListener::Core
{

enum class TypeKind
{
    ENUM,
    BOOL,
    INTEGER,
    REAL,
    POINTER,
    ARRAY,
    STRUCT,
    UNION,
    VOID,
    FUNCTION,
    COMPLEX,
    UNKNOWN,
};

constexpr std::string_view toString(TypeKind kind) noexcept
{
    switch (kind)
    {
    case TypeKind::ENUM:
        return "ENUM";
    case TypeKind::BOOL:
        return "BOOL";
    case TypeKind::INTEGER:
        return "INTEGER";
    case TypeKind::REAL:
        return "REAL";
    case TypeKind::POINTER:
        return "POINTER";
    case TypeKind::ARRAY:
        return "ARRAY";
    case TypeKind::STRUCT:
        return "STRUCT";
    case TypeKind::UNION:
        return "UNION";
    case TypeKind::VOID:
        return "VOID";
    case TypeKind::FUNCTION:
        return "FUNCTION";
    case TypeKind::COMPLEX:
        return "COMPLEX";
    case TypeKind::UNKNOWN:
        return "UNKNOWN";
    default:
        return "INVALID";
    }
}

} // namespace CodeListener::Core
