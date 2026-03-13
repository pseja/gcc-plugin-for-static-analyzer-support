#pragma once

#include <climits> // ULONG_MAX
#include <ostream> // std::ostream

namespace CodeListener::Core
{

template <typename T>
struct Id
{
    unsigned long index{ULONG_MAX};

    bool isValid() const
    {
        return index != ULONG_MAX;
    }

    static Id invalid()
    {
        return {ULONG_MAX};
    }

    bool operator==(const Id &other) const
    {
        return index == other.index;
    }
    bool operator!=(const Id &other) const
    {
        return index != other.index;
    }
    operator std::size_t() const
    {
        return index;
    }
    friend std::ostream &operator<<(std::ostream &os, const Id<T> &id)
    {
        if (!id.isValid())
        {
            return os << "<invalid>";
        }
        return os << id.index;
    }
};

} // namespace CodeListener::Core
