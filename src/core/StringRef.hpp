#pragma once

#include <cstdint> // uint32_t

namespace CodeListener::Core
{

struct StringRef
{
    uint32_t id;

    bool operator==(const StringRef &other) const;
};

} // namespace CodeListener::Core
