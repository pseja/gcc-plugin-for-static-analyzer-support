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
