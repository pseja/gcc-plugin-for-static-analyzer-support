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

class GCCDiagnosticReporter : public Core::DiagnosticReporter
{
  public:
    void setVerbosityLevel(Core::DiagnosticLevel level);
    Core::DiagnosticLevel getVerbosityLevel() const;

    void report(Core::DiagnosticLevel level, const Core::SourceLocation &source_location,
                const std::string &message) override;
    void report(Core::DiagnosticLevel level, const std::string &message,
                const std::source_location &source_location = std::source_location::current()) override;

  private:
    Core::DiagnosticLevel verbosity_level{Core::DiagnosticLevel::Warning};
};

} // namespace CodeListener::CompilerAbstractionLayer
