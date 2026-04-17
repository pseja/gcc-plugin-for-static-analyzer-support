#pragma once

#include "IAnalyzer.hpp"

struct cl_code_listener;

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Bridges the legacy cl_code_listener C API (Predator's old interface) to IAnalyzer.
 */
class LegacyPredatorBridge : public Core::IAnalyzer
{
  public:
    explicit LegacyPredatorBridge(struct cl_code_listener *listener);

    void analyze(const Core::CodeModel &model) override;

  private:
    struct cl_code_listener *listener;
};

} // namespace CodeListener::CompilerAbstractionLayer
