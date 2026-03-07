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
    void report(Core::DiagnosticLevel level, const Core::SourceLocation &source_location,
                const std::string &message) override;
    void report(Core::DiagnosticLevel level, const std::string &message,
                const std::source_location &source_location = std::source_location::current()) override;
};

} // namespace CodeListener::CompilerAbstractionLayer
