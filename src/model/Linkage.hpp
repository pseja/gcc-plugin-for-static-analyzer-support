#pragma once

#include <string_view>

namespace CodeListener::Core
{

enum class Linkage
{
    NONE,     // no linkage (e.g. local variables)
    INTERNAL, // visible only within the current translation unit
    EXTERNAL, // visible across translation units
};

constexpr std::string_view toString(Linkage linkage) noexcept
{
    switch (linkage)
    {
    case Linkage::NONE:
        return "NONE";
    case Linkage::INTERNAL:
        return "INTERNAL";
    case Linkage::EXTERNAL:
        return "EXTERNAL";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
