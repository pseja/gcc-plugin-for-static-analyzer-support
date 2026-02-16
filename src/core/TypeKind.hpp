#pragma once

namespace CodeListener
{

namespace Core
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
    CHAR, // TODO: not used by the old gcc plugin
    BOOL,
    ENUM,
    REAL,   // TODO: does float/double exist?
    STRING, // TODO: for constants (cl_cst)
};

} // namespace Core

} // namespace CodeListener
