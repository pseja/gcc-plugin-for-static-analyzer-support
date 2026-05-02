#pragma once

#include <string>
#include <vector>
#include <memory>

#include "IAnalyzer.hpp"
#include "IFrontend.hpp"
#include "JSONImporter.hpp"

namespace CodeListener::Exporters
{

/**
 * Frontend that loads an already-serialised CodeModel from a JSON file.
 *
 * Used by the cl_analyze standalone tool. The file is loaded once in run(), then every analyzer in the provided list is
 * invoked against the model.
 */
class JSONFrontend : public Core::IFrontend
{
  public:
    explicit JSONFrontend(std::string json_path);

    bool run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx) override;

  private:
    std::string json_path;
};

} // namespace CodeListener::Exporters
