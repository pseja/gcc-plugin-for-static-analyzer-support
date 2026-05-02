/**
 * @file TypeKind.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines type categories tracked by the CodeModel.
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

enum class TypeKind
{
    ENUM,
    BOOL,
    INTEGER,
    REAL,
    POINTER,
    ARRAY,
    STRUCT,
    UNION,
    VOID,
    FUNCTION,
    COMPLEX,
    UNKNOWN,
};

constexpr std::string_view toString(TypeKind kind) noexcept
{
    switch (kind)
    {
    case TypeKind::ENUM:
        return "ENUM";
    case TypeKind::BOOL:
        return "BOOL";
    case TypeKind::INTEGER:
        return "INTEGER";
    case TypeKind::REAL:
        return "REAL";
    case TypeKind::POINTER:
        return "POINTER";
    case TypeKind::ARRAY:
        return "ARRAY";
    case TypeKind::STRUCT:
        return "STRUCT";
    case TypeKind::UNION:
        return "UNION";
    case TypeKind::VOID:
        return "VOID";
    case TypeKind::FUNCTION:
        return "FUNCTION";
    case TypeKind::COMPLEX:
        return "COMPLEX";
    case TypeKind::UNKNOWN:
        return "UNKNOWN";
    default:
        return "INVALID";
    }
}

} // namespace CodeListener::Core
