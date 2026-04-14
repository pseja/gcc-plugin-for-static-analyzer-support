#pragma once

#include <string_view>

namespace CodeListener::Core
{

enum class Scope
{
    GLOBAL,
    STATIC,
    FUNCTION,
};

constexpr std::string_view toString(Scope scope) noexcept
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

} // namespace CodeListener::Core
