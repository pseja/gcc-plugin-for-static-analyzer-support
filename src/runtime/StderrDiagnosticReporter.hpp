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
    void report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) override;
    void report(DiagnosticLevel level, const std::string &message,
                const std::source_location &src = std::source_location::current()) override;

    /**
     * Returns true if any Error or Fatal diagnostic was emitted.
     */
    bool hadError() const;

  private:
    bool had_error{false};

    static const char *dlvlToString(DiagnosticLevel level);
};

} // namespace CodeListener::Core
