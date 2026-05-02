#include "LegacyPredatorBridge.hpp"

#include "PredatorAdapter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

LegacyPredatorBridge::LegacyPredatorBridge(struct cl_code_listener *listener) : listener(listener)
{
}

bool LegacyPredatorBridge::analyze(const Core::CodeModel &model, AnalysisContext &ctx)
{
    // the legacy Predator C API has its own internal reporter
    (void)ctx;

    CodeListener::Adapters::PredatorAdapter pa(model, listener);
    pa.emit();
    return true;
}

} // namespace CodeListener::CompilerAbstractionLayer
