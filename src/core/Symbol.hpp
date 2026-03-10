#pragma once

#include <cstdint> // uint32_t

namespace CodeListener::Core
{

struct Symbol
{
    uint32_t id;

    bool operator==(const Symbol &other) const;
};

} // namespace CodeListener::Core
