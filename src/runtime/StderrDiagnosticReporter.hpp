/**
 * @file StderrDiagnosticReporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares a diagnostic reporter that writes messages to standard error.
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

#include "DiagnosticLevel.hpp"
#include "DiagnosticReporter.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

/**
 * A plain stderr implementation of DiagnosticReporter.
 *
 * Used by standalone tools and any context where GCC's diagnostic infrastructure is not available.
 */
class StderrDiagnosticReporter : public DiagnosticReporter
{
  public:
    /** @copydoc CodeListener::Core::DiagnosticReporter::report(DiagnosticLevel, const SourceLocation&, const std::string&) */
    void report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) override;

    /** @copydoc CodeListener::Core::DiagnosticReporter::report(DiagnosticLevel, const std::string&, const std::source_location&) */
    void report(DiagnosticLevel level, const std::string &message,
                const std::source_location &loc = std::source_location::current()) override;

    /**
     * Returns true if any Error or Fatal diagnostic was emitted.
     *
     * @return True if at least one error-level diagnostic was reported.
     */
    bool hadError() const;

  private:
    /** Tracks whether an error-or-worse diagnostic was already emitted. */
    bool had_error{false};

    /**
     * Convert a diagnostic level to a human-readable label.
     *
     * @param level Diagnostic severity level.
     *
     * @return Null-terminated string label for the level.
     */
    static const char *dlvlToString(DiagnosticLevel level);
};

} // namespace CodeListener::Core
