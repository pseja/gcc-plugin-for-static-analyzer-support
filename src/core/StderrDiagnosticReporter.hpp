#pragma once

#include <cstdio>

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
    void report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) override
    {
        std::fprintf(stderr, "%s:%d:%d: %s: %s\n", loc.file.empty() ? "<unknown>" : loc.file.c_str(), loc.line,
                     loc.column, dlvlToString(level), message.c_str());
    }

    void report(DiagnosticLevel level, const std::string &message,
                const std::source_location & /*src*/ = std::source_location::current()) override
    {
        std::fprintf(stderr, "%s: %s\n", dlvlToString(level), message.c_str());
    }

  private:
    static const char *dlvlToString(DiagnosticLevel level)
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
};

} // namespace CodeListener::Core
