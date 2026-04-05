#pragma once

#include "AnalysisManager.hpp"
#include "CodeModel.hpp"
#include "CodeModelVisitor.hpp"

namespace CodeListener::Exporters
{

/**
 * Base class for all exporters.
 *
 * Derives from CodeModelVisitor: exportModel() drives the walk, which in turn
 * fires the onBeginX/onEndX/onVisitX hooks that concrete exporters override.
 *
 * Every exporter owns an AnalysisManager so annotations are always available
 * via analysis_manager.getAnnotation<T>(model) - no null checks required.
 * Annotations are computed lazily on first access.
 */
class Exporter : public Core::CodeModelVisitor
{
  public:
    virtual ~Exporter() = default;

    void exportModel(const Core::CodeModel &model) { walk(model); }

  protected:
    AnnotationServices::AnalysisManager analysis_manager;
};

} // namespace CodeListener::Exporters
