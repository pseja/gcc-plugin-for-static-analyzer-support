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

namespace std
{

template <typename T>
struct hash<CodeListener::Core::Id<T>>
{
    std::size_t operator()(const CodeListener::Core::Id<T> &id) const
    {
        return std::hash<unsigned long>{}(id.index);
    }
};

} // namespace std
