#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "AnalysisManager.hpp"
#include "CodeModel.hpp"
#include "Exporter.hpp"

namespace CodeListener::Exporters
{

class JSONExporter : public Exporter
{
  public:
    explicit JSONExporter(std::ostream &os, AnnotationServices::AnalysisManager *manager = nullptr);
    explicit JSONExporter(const std::string &filepath, AnnotationServices::AnalysisManager *manager = nullptr);

    void exportModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;
    AnnotationServices::AnalysisManager *analysis_manager;
};

} // namespace CodeListener::Exporters
