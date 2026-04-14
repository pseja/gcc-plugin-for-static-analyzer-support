#include "Exporter.hpp"

namespace CodeListener::Exporters
{

void Exporter::exportModel(const Core::CodeModel &model)
{
    walk(model);
}

} // namespace CodeListener::Exporters
