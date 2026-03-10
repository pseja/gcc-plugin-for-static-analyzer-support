#pragma once

#include "Symbol.hpp"

namespace CodeListener::Core
{

bool Symbol::operator==(const Symbol &other) const
{
    return id == other.id;
}

} // namespace CodeListener::Core
