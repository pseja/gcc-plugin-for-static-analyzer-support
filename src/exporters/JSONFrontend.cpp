#include "JSONFrontend.hpp"

#include "CodeModel.hpp"
#include "JSONImporter.hpp"

namespace CodeListener::Exporters
{

JSONFrontend::JSONFrontend(std::string json_path) : json_path(std::move(json_path))
{
}

bool JSONFrontend::run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx)
{
    Core::CodeModel model = JSONImporter::importFromFile(json_path);

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

} // namespace CodeListener::Exporters
