#include "GCCFrontend.hpp"

#include "CodeModel.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

GCCFrontend::GCCFrontend(const Core::CodeModel &model) : model(model)
{
}

bool GCCFrontend::run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx)
{
    bool all_succeeded = true;
    for (auto &analyzer : analyzers)
    {
        if (!analyzer->analyze(model, ctx))
        {
            all_succeeded = false;
        }
    }
    return all_succeeded;
}

} // namespace CodeListener::CompilerAbstractionLayer
