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
#include <sstream>

namespace CodeListener::Core
{

/** Captures a best-effort source position for a model entity or diagnostic. */
struct SourceLocation
{
    /** Source file path or placeholder when the file is unknown. */
    std::string file{"<unknown>"};

    /** Function name associated with the location when known. */
    std::string function{"<unknown>"};

    /** One-based source line number, or `0` when unavailable. */
    int line{0};

    /** One-based source column number, or `0` when unavailable. */
    int column{0};

    /** Optional backend-specific handle pointing to the native source location object. */
    void *native_handle{nullptr};

    /** Constructs an unknown source location placeholder. */
    SourceLocation() = default;

    /**
     * Constructs a location with explicit file, function, and coordinates.
     *
     * @param file Source file path.
     * @param func Function name associated with the location.
     * @param line One-based line number.
     * @param column One-based column number.
     * @param native_handle Optional backend-specific location handle.
     */
    SourceLocation(std::string file, std::string func, int line, int column, void *native_handle = nullptr);

    /**
     * Constructs a location without explicit function information.
     *
     * @param file Source file path.
     * @param line One-based line number.
     * @param column One-based column number.
     * @param native_handle Optional backend-specific location handle.
     */
    SourceLocation(std::string file, int line, int column, void *native_handle = nullptr);
};

/**
 * Format a source location as `file:function:line:column`.
 *
 * @param loc Location to render.
 *
 * @return Owning string containing the formatted representation.
 */
inline std::string toString(const SourceLocation &loc)
{
    std::ostringstream oss;
    oss << loc.file << ':' << loc.function << ':' << loc.line << ':' << loc.column;
    return oss.str();
}

} // namespace CodeListener::Core
