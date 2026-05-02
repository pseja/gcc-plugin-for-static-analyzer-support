#pragma once

#include "AnalysisKey.hpp"
#include "Annotation.hpp"

namespace CodeListener::AnnotationServices
{

// CRTP base class for annotations
template <typename Derived>
struct AnnotationBase : public Annotation
{
    static AnalysisKey Key;
};

template <typename Derived>
AnalysisKey AnnotationBase<Derived>::Key;

} // namespace CodeListener::AnnotationServices
