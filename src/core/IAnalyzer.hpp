#pragma once

#include "AnalysisContext.hpp"
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
     * Run the analysis on the fully-populated model.
     *
     * @param model The CodeModel to analyze.
     * @param ctx   Services: GCC-formatted reporter, shared annotation cache, and export helpers.
     */
    virtual void analyze(const CodeModel &model, AnalysisContext &ctx) = 0;
};

} // namespace CodeListener::Core
