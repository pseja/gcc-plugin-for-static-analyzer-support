#pragma once

#include <unordered_map>
#include <vector>

#include "CodeModel.hpp"

namespace CodeListener::Exporters
{

class ModelMerger
{
  public:
    static Core::CodeModel merge(std::vector<Core::CodeModel> models);
};

} // namespace CodeListener::Exporters
