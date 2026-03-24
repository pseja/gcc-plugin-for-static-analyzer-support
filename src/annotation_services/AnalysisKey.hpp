#pragma once

namespace CodeListener::AnnotationServices
{

struct AnalysisKey
{
    // Intentionally empty, as it's only used as a unique memory address for identifying analyses in the
    // AnalysisManager's cache.

    // inspired by https://llvm.org/doxygen/structllvm_1_1AnalysisKey.html
};

} // namespace CodeListener::AnnotationServices
