#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "Exporter.hpp"

namespace CodeListener::Exporters
{

class JSONExporter : public Exporter
{
  public:
    explicit JSONExporter(std::ostream &os);
    explicit JSONExporter(const std::string &filepath);

  protected:
    // JSON is a batch format: onEndModel assembles and dumps the whole document.
    // Fine-grained hooks (onVisitType etc.) are not needed here.
    void onEndModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;
};

} // namespace CodeListener::Exporters
