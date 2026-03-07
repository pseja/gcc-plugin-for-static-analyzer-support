#pragma once

#include "CodeModel.hpp"

namespace CodeListener::Exporters
{

class Exporter
{
  public:
    virtual ~Exporter() = default;

    virtual void exportModel(const Core::CodeModel &model) = 0;
};

} // namespace CodeListener::Exporters
