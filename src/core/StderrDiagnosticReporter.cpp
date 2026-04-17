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
