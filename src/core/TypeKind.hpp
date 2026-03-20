#pragma once

#include <string>

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

inline std::string toString(TypeKind kind)
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
