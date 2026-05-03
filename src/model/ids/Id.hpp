/**
 * @file Id.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the common strongly typed identifier wrapper used by model entities.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <climits>    // ULONG_MAX
#include <functional> // std::hash
#include <ostream>    // std::ostream

namespace CodeListener::Core
{

/**
 * Strongly typed identifier wrapper used for entities stored in indexed pools.
 *
 * @tparam T Tag type distinguishing incompatible identifier domains.
 */
template <typename T>
struct Id
{
    /** Raw pool index, or `ULONG_MAX` when the identifier is invalid. */
    unsigned long index{ULONG_MAX};

    /** @return Whether the identifier refers to a valid pool entry. */
    bool isValid() const
    {
        return index != ULONG_MAX;
    }

    /** @return Sentinel invalid identifier for this tag type. */
    static Id invalid()
    {
        return {ULONG_MAX};
    }

    /** Compare two identifiers of the same domain for equality. */
    bool operator==(const Id &other) const
    {
        return index == other.index;
    }

    /** Compare two identifiers of the same domain for inequality. */
    bool operator!=(const Id &other) const
    {
        return index != other.index;
    }

    /** Implicit conversion to the raw index value. */
    operator std::size_t() const
    {
        return index;
    }

    /** Stream the identifier as its index or as `<invalid>` for sentinel values. */
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

namespace std
{

/** Hash specialization enabling strongly typed ids in unordered containers. */
template <typename T>
struct hash<CodeListener::Core::Id<T>>
{
    /** Compute the hash from the wrapped raw index. */
    std::size_t operator()(const CodeListener::Core::Id<T> &id) const
    {
        return std::hash<unsigned long>{}(id.index);
    }
};

} // namespace std
