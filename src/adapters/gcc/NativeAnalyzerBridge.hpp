#pragma once

#include <string>

#include <cl_native_analyzer_api.h>

#include "IAnalyzer.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Bridges cl_native_analyzer_api_t (C ABI, loaded via dlopen) to IAnalyzer.
 */
class NativeAnalyzerBridge : public Core::IAnalyzer
{
  public:
    explicit NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args = {});

    void analyze(const Core::CodeModel &model, AnalysisContext &ctx) override;

  private:
    const cl_native_analyzer_api_t *api;
    std::string args;
};

} // namespace CodeListener::CompilerAbstractionLayer
