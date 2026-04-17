#ifndef ANALYSIS_CONTEXT_HPP
#define ANALYSIS_CONTEXT_HPP

#ifdef __cplusplus

#include <string>

#include "AnalysisManager.hpp"
#include "DiagnosticReporter.hpp"
#include "DOTVerbosity.hpp"

namespace CodeListener
{

namespace Core
{
class CodeModel;
}

/**
 * Bundles all services an external analyzer might need to analyze the CodeModel.
 */
class AnalysisContext
{
  public:
    Core::DiagnosticReporter &reporter;
    AnnotationServices::AnalysisManager &analysis_manager;

    AnalysisContext(Core::DiagnosticReporter &reporter, AnnotationServices::AnalysisManager &analysis_manager)
        : reporter(reporter), analysis_manager(analysis_manager)
    {
    }

    AnalysisContext(const AnalysisContext &) = delete;
    AnalysisContext &operator=(const AnalysisContext &) = delete;

    /**
     * Export the model as a DOT CFG graph to path.
     */
    void exportDot(const Core::CodeModel &model, const std::string &path,
                   Exporters::DotVerbosity verbosity = Exporters::DotVerbosity::CLEAN);

    /**
     * Export the model as a pretty-printed 3-address-code listing to path.
     */
    void exportPP(const Core::CodeModel &model, const std::string &path);

    /**
     * Serialize the model to the internal JSON format at path.
     */
    void exportJson(const Core::CodeModel &model, const std::string &path);
};

} // namespace CodeListener

#endif /* __cplusplus */
#endif /* ANALYSIS_CONTEXT_HPP */
