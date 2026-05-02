#pragma once

#include <istream>
#include <string>

#include "CodeModel.hpp"

namespace CodeListener::Exporters
{

class JSONImporter
{
  public:
    static Core::CodeModel importFromFile(const std::string &filepath);
    static Core::CodeModel importFromStream(std::istream &is);
};

} // namespace CodeListener::Exporters
