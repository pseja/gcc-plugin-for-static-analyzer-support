/**
 * @file AccessorKind.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines accessor kinds used in compound variable references.
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

#include <string_view>

namespace CodeListener::Core
{

enum class AccessorKind
{
    DEREF,      // *ptr
    ARRAY,      // ptr[index] or arr[index]
    FIELD,      // obj.field or ptr->field
    ADDRESS_OF, // &expr
    OFFSET,     // (char*)ptr + offset (pointer arithmetic)
    BIT_SLICE,  // expr[start:end] (bit field extraction)
};

constexpr std::string_view toString(AccessorKind accessor_kind) noexcept
{
    switch (accessor_kind)
    {
    case AccessorKind::DEREF:
        return "DEREF";
    case AccessorKind::ARRAY:
        return "ARRAY";
    case AccessorKind::FIELD:
        return "FIELD";
    case AccessorKind::ADDRESS_OF:
        return "ADDRESS_OF";
    case AccessorKind::OFFSET:
        return "OFFSET";
    case AccessorKind::BIT_SLICE:
        return "BIT_SLICE";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
