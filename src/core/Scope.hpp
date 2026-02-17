#pragma once

#include <string>

namespace CodeListener
{

namespace Core
{

enum class Scope
{
    GLOBAL,
    STATIC,
    FUNCTION,
};

inline std::string toString(Scope scope)
{
    switch (scope)
    {
    case Scope::GLOBAL:
        return "GLOBAL";
    case Scope::STATIC:
        return "STATIC";
    case Scope::FUNCTION:
        return "FUNCTION";
    default:
        return "UNKNOWN";
    }
}

} // namespace Core

} // namespace CodeListener
