/**
 * @file SourceLocation.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements source location utilities.
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

#include <string>

#include "SourceLocation.hpp"

namespace CodeListener::Core
{

SourceLocation::SourceLocation(std::string file, std::string func, int line, int column, void *native_handle)
    : file(std::move(file)), function(std::move(func)), line(line), column(column), native_handle(native_handle)
{
}
SourceLocation::SourceLocation(std::string file, int line, int column, void *native_handle)
    : file(std::move(file)), line(line), column(column), native_handle(native_handle)
{
}

} // namespace CodeListener::Core
