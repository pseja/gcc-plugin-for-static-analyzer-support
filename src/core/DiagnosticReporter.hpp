#pragma once

#include <source_location>
#include <string>

#include "SourceLocation.hpp"

namespace CodeListener
{

namespace Core
{

enum class DiagnosticLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class DiagnosticReporter
{
  public:
    virtual ~DiagnosticReporter() = default;

    virtual void report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) = 0;
    virtual void report(DiagnosticLevel level, const std::string &message,
                        const std::source_location &loc = std::source_location::current()) = 0;
};

} // namespace Core

} // namespace CodeListener
