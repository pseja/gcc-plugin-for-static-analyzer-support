#include "AnalysisContext.hpp"

#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "PPExporter.hpp"

namespace CodeListener
{

void AnalysisContext::exportDot(const Core::CodeModel &model, const std::string &path,
                                Exporters::DotVerbosity verbosity)
{
    Exporters::DOTExporter exporter(path, verbosity);
    exporter.exportModel(model);
}

void AnalysisContext::exportPP(const Core::CodeModel &model, const std::string &path)
{
    Exporters::PPExporter exporter(path);
    exporter.exportModel(model);
}

void AnalysisContext::exportJson(const Core::CodeModel &model, const std::string &path)
{
    Exporters::JSONExporter exporter(path);
    exporter.exportModel(model);
}

} // namespace CodeListener
