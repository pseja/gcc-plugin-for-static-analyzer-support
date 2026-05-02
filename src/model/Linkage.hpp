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

enum class Linkage
{
    NONE,     // no linkage (e.g. local variables)
    INTERNAL, // visible only within the current translation unit
    EXTERNAL, // visible across translation units
};

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
