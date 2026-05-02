#pragma once

#include <vector>
#include <memory>

#include "AnalysisContext.hpp"
#include "IAnalyzer.hpp"
#include "IFrontend.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Frontend that receives a fully-processed CodeModel from the GCC pass manager (produced by GCCAdapter) and runs all
 * registered analyzers against it.
 */
class GCCFrontend : public Core::IFrontend
{
  public:
    /**
     * @param model The CodeModel populated by GCCAdapter during the GCC pass.
     */
    explicit GCCFrontend(const Core::CodeModel &model);

    bool run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx) override;

  private:
    const Core::CodeModel &model;
};

} // namespace CodeListener::CompilerAbstractionLayer
