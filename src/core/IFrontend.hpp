#pragma once

#include <memory>
#include <vector>

#include "AnalysisContext.hpp"
#include "IAnalyzer.hpp"

namespace CodeListener::Core
{

/**
 * Abstract interface for compiler/tool frontends.
 *
 * A frontend is responsible for populating a CodeModel and driving one full analysis pass over it.
 *
 * Concrete implementations include:
 *   - GCCFrontend  - receives GIMPLE from the GCC pass manager (cl_gcc plugin)
 *   - JSONFrontend - loads a pre-built CodeModel from a JSON file (cl_analyze)
 *
 * Inspired by LLVM's FrontendAction/ASTConsumer contract.
 */
class IFrontend
{
  public:
    virtual ~IFrontend() = default;

    /**
     * Run all registered analyzers on the model produced by this frontend.
     *
     * @param analyzers Ordered list of analyzer backends to invoke.
     * @param ctx       Shared analysis services (reporter, annotation cache, export helpers).
     *
     * @return true if every analyzer succeeded; false if any reported an error
     */
    virtual bool run(std::vector<std::unique_ptr<IAnalyzer>> &analyzers, AnalysisContext &ctx) = 0;
};

} // namespace CodeListener::Core
