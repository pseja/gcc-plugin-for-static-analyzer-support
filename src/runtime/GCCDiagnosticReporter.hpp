/**
 * @file GCCDiagnosticReporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares a diagnostic reporter that forwards messages to GCC.
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

#include <gcc-plugin.h>
#include <line-map.h>

#include "DiagnosticReporter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/** Diagnostic reporter implementation that forwards messages to GCC diagnostics. */
class GCCDiagnosticReporter : public Core::DiagnosticReporter
{
  public:
    /**
     * Set the minimum severity that should be printed through the debug side channel.
     *
     * @param level Requested verbosity threshold.
     */
    void setVerbosityLevel(Core::DiagnosticLevel level);

    /**
     * Return the currently configured verbosity threshold.
     *
     * @return Current verbosity level.
     */
    Core::DiagnosticLevel getVerbosityLevel() const;

    /**
     * Report a diagnostic tied to a translated source location through GCC's diagnostics.
     *
     * @param level Diagnostic severity.
     * @param source_location Source location associated with the diagnostic.
     * @param message Human-readable diagnostic text.
     */
    void report(Core::DiagnosticLevel level, const Core::SourceLocation &source_location,
                const std::string &message) override;

    /**
     * Report a diagnostic tied to an internal source location through GCC's diagnostics.
     *
     * @param level Diagnostic severity.
     * @param message Human-readable diagnostic text.
     * @param source_location Internal C++ source location used for debug logging.
     */
    void report(Core::DiagnosticLevel level, const std::string &message,
                const std::source_location &source_location = std::source_location::current()) override;

  private:
    /** Current verbosity threshold for development-only debug messages. */
    Core::DiagnosticLevel verbosity_level{Core::DiagnosticLevel::Warning};
};

} // namespace CodeListener::CompilerAbstractionLayer
