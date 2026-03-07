#pragma once

#include <string>

namespace CodeListener::Core
{

enum class TypeKind
{
    UNKNOWN,
    VOID,
    POINTER,
    STRUCT,
    UNION,
    ARRAY,
    FUNCTION,
    INTEGER,
    BOOL,
    ENUM,
    REAL,
    COMPLEX,
};

inline std::string toString(TypeKind kind)
{
    switch (kind)
    {
    case TypeKind::UNKNOWN:
        return "UNKNOWN";
    case TypeKind::VOID:
        return "VOID";
    case TypeKind::POINTER:
        return "POINTER";
    case TypeKind::STRUCT:
        return "STRUCT";
    case TypeKind::UNION:
        return "UNION";
    case TypeKind::ARRAY:
        return "ARRAY";
    case TypeKind::FUNCTION:
        return "FUNCTION";
    case TypeKind::INTEGER:
        return "INTEGER";
    case TypeKind::BOOL:
        return "BOOL";
    case TypeKind::ENUM:
        return "ENUM";
    case TypeKind::REAL:
        return "REAL";
    case TypeKind::COMPLEX:
        return "COMPLEX";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
