/**
 * @file SourceLocation.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares source file and coordinate information attached to model entities.
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

#include <string>
#include <string_view>
#include <sstream>

namespace CodeListener::Core
{

struct SourceLocation
{
    std::string file{"<unknown>"};
    std::string function{"<unknown>"};
    int line{0};
    int column{0};
    void *native_handle{nullptr};

    SourceLocation() = default;
    SourceLocation(std::string file, std::string func, int line, int column, void *native_handle = nullptr);
    SourceLocation(std::string file, int line, int column, void *native_handle = nullptr);
};

inline std::string_view toString(const SourceLocation &loc)
{
    std::ostringstream oss;
    oss << loc.file << ':' << loc.function << ':' << loc.line << ':' << loc.column;
    return oss.str();
}

} // namespace CodeListener::Core
