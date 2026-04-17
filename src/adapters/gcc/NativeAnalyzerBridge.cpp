#include "NativeAnalyzerBridge.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

NativeAnalyzerBridge::NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args)
    : api_(api), args_(std::move(args))
{
}

void NativeAnalyzerBridge::analyze(const Core::CodeModel &model)
{
    if (api_ && api_->analyze)
    {
        api_->analyze(model, args_.empty() ? nullptr : args_.c_str());
    }
}

} // namespace CodeListener::CompilerAbstractionLayer
