#include "LegacyPredatorBridge.hpp"

#include "PredatorAdapter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

LegacyPredatorBridge::LegacyPredatorBridge(struct cl_code_listener *listener) : listener(listener)
{
}

void LegacyPredatorBridge::analyze(const Core::CodeModel &model)
{
    CodeListener::Adapters::PredatorAdapter pa(model, listener);
    pa.emit();
}

} // namespace CodeListener::CompilerAbstractionLayer
