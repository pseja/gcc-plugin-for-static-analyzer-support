#pragma once

#include <string>

#include <cl_native_analyzer_api.h>

#include "IAnalyzer.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Bridges cl_native_analyzer_api_t (C ABI, loaded via dlopen) to IAnalyzer.
 *
 * A shared library that exports cl_get_native_api() and whose api_version
 * matches CL_NATIVE_API_VERSION can be wrapped in this class and handed to
 * PluginContext without any further knowledge of the C API.
 */
class NativeAnalyzerBridge : public Core::IAnalyzer
{
  public:
    explicit NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args = {});

    void analyze(const Core::CodeModel &model) override;

  private:
    const cl_native_analyzer_api_t *api_;
    std::string args_;
};

} // namespace CodeListener::CompilerAbstractionLayer
