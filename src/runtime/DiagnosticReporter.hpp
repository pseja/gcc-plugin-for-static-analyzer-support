/**
 * @file DiagnosticReporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the diagnostic reporting interface shared by frontends and analyzers.
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

#include <source_location>
#include <string>

#include "DiagnosticLevel.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

/**
 * Abstract sink for diagnostics emitted by frontends, exporters, and analyzers.
 */
class DiagnosticReporter
{
  public:
    virtual ~DiagnosticReporter() = default;

    /**
     * Emit a diagnostic message associated with a specific source location.
     *
     * @param level   Severity of the diagnostic.
     * @param loc     Source location relevant to the message.
     * @param message Human-readable diagnostic text.
     */
    virtual void report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) = 0;

    /**
     * Emit a diagnostic message without a CodeModel source location.
     *
     * @param level   Severity of the diagnostic.
     * @param message Human-readable diagnostic text.
     * @param loc     C++ source location of the call site (auto-captured).
     */
    virtual void report(DiagnosticLevel level, const std::string &message,
                        const std::source_location &loc = std::source_location::current()) = 0;
};

} // namespace CodeListener::Core
