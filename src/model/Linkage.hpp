/**
 * @file Linkage.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines linkage kinds recorded for functions and variables.
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

/** Describes the linkage recorded for a named entity. */
enum class Linkage
{
    NONE,     /**< No linkage, for example for local variables. */
    INTERNAL, /**< Visible only within the current translation unit. */
    EXTERNAL, /**< Visible across translation units. */
};

/**
 * Convert a linkage kind to its stable textual name.
 *
 * @param linkage Linkage kind to stringify.
 *
 * @return Short symbolic name used in diagnostics and exports.
 */
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
