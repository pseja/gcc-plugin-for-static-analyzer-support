#include <cstdint> // uintptr_t

#include <gcc-plugin.h>
#include <diagnostic-core.h> // warning_at, error_at, inform, fatal_error

#include "GCCDiagnosticReporter.hpp"

namespace CodeListener
{

namespace GCC_ADAPTER
{

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
        inform(gcc_source_location, "%s", message.c_str());
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
    if (level == CodeListener::Core::DiagnosticLevel::Debug)
    {
        CodeListener::Core::SourceLocation sloc(source_location.file_name(), source_location.function_name(),
                                                source_location.line(), source_location.column());
        report(level, sloc, msg);
    }
    else
    {
        report(level, CodeListener::Core::SourceLocation(), msg);
    }
}

} // namespace GCC_ADAPTER

} // namespace CodeListener
