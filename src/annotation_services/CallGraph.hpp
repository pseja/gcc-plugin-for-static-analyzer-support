#pragma once

#include <unordered_map>
#include <vector>

#include "AnalysisKey.hpp"
#include "Annotation.hpp"
#include "CodeModel.hpp"
#include "FunctionId.hpp"

namespace CodeListener::AnnotationServices
{

struct CallGraph : public Annotation
{
    static AnalysisKey Key;

    // outgoing edges mapping caller function ID to a list of callee function IDs
    std::unordered_map<Core::FunctionId, std::vector<Core::FunctionId>> calls;
    // incoming edges mapping callee function ID to a list of caller function IDs
    std::unordered_map<Core::FunctionId, std::vector<Core::FunctionId>> called_by;

    static CallGraph build(const Core::CodeModel &model);
};

} // namespace CodeListener::AnnotationServices
