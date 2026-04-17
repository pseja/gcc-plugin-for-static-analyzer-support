#include "NativeAnalyzerBridge.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

NativeAnalyzerBridge::NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args)
    : api(api), args(std::move(args))
{
}

void NativeAnalyzerBridge::analyze(const Core::CodeModel &model, AnalysisContext &ctx)
{
    if (api && api->analyze)
    {
        api->analyze(model, ctx, args.empty() ? nullptr : args.c_str());
    }
}

} // namespace CodeListener::CompilerAbstractionLayer
