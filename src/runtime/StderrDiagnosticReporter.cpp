/**
 * @file StderrDiagnosticReporter.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements a diagnostic reporter that writes messages to standard error.
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

#include <cstdio>
#include <source_location>

#include "StderrDiagnosticReporter.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

void StderrDiagnosticReporter::report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message)
{
    if (level >= DiagnosticLevel::Error)
    {
        had_error = true;
    }

    std::fprintf(stderr, "%s:%d:%d: %s: %s\n", loc.file.empty() ? "<unknown>" : loc.file.c_str(), loc.line, loc.column,
                 dlvlToString(level), message.c_str());
}

void StderrDiagnosticReporter::report(DiagnosticLevel level, const std::string &message, const std::source_location &)
{
    if (level >= DiagnosticLevel::Error)
    {
        had_error = true;
    }

    std::fprintf(stderr, "%s: %s\n", dlvlToString(level), message.c_str());
}

bool StderrDiagnosticReporter::hadError() const
{
    return had_error;
}

const char *StderrDiagnosticReporter::dlvlToString(DiagnosticLevel level)
{
    switch (level)
    {
    case DiagnosticLevel::Debug:
        return "debug";
    case DiagnosticLevel::Info:
        return "note";
    case DiagnosticLevel::Warning:
        return "warning";
    case DiagnosticLevel::Error:
        return "error";
    case DiagnosticLevel::Fatal:
        return "fatal error";
    }
    return "note";
}

} // namespace CodeListener::Core
