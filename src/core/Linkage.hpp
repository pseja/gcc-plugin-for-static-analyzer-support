#pragma once

#include <string>

namespace CodeListener
{

namespace Core
{

enum class Linkage
{
    NONE,     // no linkage (e.g. local variables)
    INTERNAL, // visible only within the current translation unit
    EXTERNAL, // visible across translation units
};

inline std::string toString(Linkage linkage)
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

} // namespace Core

} // namespace CodeListener
