#pragma once

#include "CodeModel.hpp"

namespace CodeListener::Core
{

/**
 * Abstract interface implemented by every analyzer backend.
 *
 * Concrete implementations are:
 *   - NativeAnalyzerBridge - wraps cl_native_analyzer_api_t (C-ABI shared lib)
 *   - LegacyPredatorBridge - wraps cl_code_listener (old Predator C API)
 */
class IAnalyzer
{
  public:
    virtual ~IAnalyzer() = default;

    /**
     * Run the analysis on the fully-populated model for one translation unit
     * (or a merged multi-TU model when invoked via cl_analyze).
     */
    virtual void analyze(const CodeModel &model) = 0;
};

} // namespace CodeListener::Core
