/**
 * @file GCCDiagnosticReporter.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements a diagnostic reporter that forwards messages to GCC.
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

#include <cstdint> // uintptr_t

#include <gcc-plugin.h>
#include <diagnostic-core.h> // warning_at, error_at, inform, fatal_error

#include "GCCDiagnosticReporter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Shorten an absolute path to a project-relative form for cleaner diagnostics.
 *
 * @param path Absolute or relative filesystem path.
 *
 * @return Shortened path relative to the project src/ directory, or basename.
 */
static std::string getProjectRelativePath(const std::string &path)
{
    size_t pos = path.find("/src/");
    if (pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }
    if (path.find("src/") == 0)
    {
        return path;
    }

    pos = path.rfind('/');
    if (pos != std::string::npos)
    {
        return path.substr(pos + 1);
    }
    return path;
}

void GCCDiagnosticReporter::report(Core::DiagnosticLevel level, const Core::SourceLocation &source_location,
                                   const std::string &msg)
{
    if (level < verbosity_level)
    {
        return;
    }

    location_t gcc_source_location = UNKNOWN_LOCATION;
    if (source_location.native_handle)
    {
        gcc_source_location = static_cast<location_t>((uintptr_t)source_location.native_handle);
    }

    std::string message = msg;

    if (gcc_source_location == UNKNOWN_LOCATION && !source_location.file.empty())
    {
        message = source_location.file;
        if (source_location.line > 0)
        {
            message += ":" + std::to_string(source_location.line);
            if (source_location.column > 0)
            {
                message += ":" + std::to_string(source_location.column);
            }
        }
        message += " " + msg;
    }

    switch (level)
    {
    case Core::DiagnosticLevel::Debug:
        fprintf(stderr, "\033[90m%s:%-4d | %15s | %s\033[0m\n",
                source_location.file.empty() ? "unknown" : getProjectRelativePath(source_location.file).c_str(),
                source_location.line, source_location.function.empty() ? "unknown" : source_location.function.c_str(),
                msg.c_str());
        break;
    case Core::DiagnosticLevel::Info:
        if (gcc_source_location == UNKNOWN_LOCATION)
        {
            // fall back to stderr, when GCC would print `note: <unknown>`
            fprintf(stderr, "\033[1;36mnote:\033[0m %s\n", msg.c_str());
        }
        else
        {
            inform(gcc_source_location, "%s", message.c_str());
        }
        break;
    case Core::DiagnosticLevel::Warning:
        warning_at(gcc_source_location, 0, "%s", message.c_str());
        break;
    case Core::DiagnosticLevel::Error:
        error_at(gcc_source_location, "%s", message.c_str());
        break;
    case Core::DiagnosticLevel::Fatal:
        fatal_error(gcc_source_location, "%s", message.c_str());
        break;
    }
}

void GCCDiagnosticReporter::report(CodeListener::Core::DiagnosticLevel level, const std::string &msg,
                                   const std::source_location &source_location)
{
    if (level < verbosity_level)
    {
        return;
    }

    if (level == CodeListener::Core::DiagnosticLevel::Debug)
    {
        CodeListener::Core::SourceLocation sloc(source_location.file_name(), source_location.function_name(),
                                                source_location.line(), source_location.column());
        report(level, sloc, msg);
    }
    else
    {
        CodeListener::Core::SourceLocation sloc;
        sloc.native_handle = reinterpret_cast<void *>(static_cast<uintptr_t>(input_location));
        report(level, sloc, msg);
    }
}

void GCCDiagnosticReporter::setVerbosityLevel(Core::DiagnosticLevel level)
{
    verbosity_level = level;
}
Core::DiagnosticLevel GCCDiagnosticReporter::getVerbosityLevel() const
{
    return verbosity_level;
}

} // namespace CodeListener::CompilerAbstractionLayer
