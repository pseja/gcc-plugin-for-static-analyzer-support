#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Exporter.hpp"
#include "../../core/CodeModel.hpp"

namespace CodeListener
{

namespace Exporters
{

class JSONExporter : public Exporter
{
  public:
    explicit JSONExporter(std::ostream &os);
    explicit JSONExporter(const std::string &filepath);

    void exportModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;
};

} // namespace Exporters

} // namespace CodeListener
