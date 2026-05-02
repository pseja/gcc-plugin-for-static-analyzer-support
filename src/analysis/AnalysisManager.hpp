#pragma once

#include <memory>
#include <unordered_map>

#include "AnalysisKey.hpp"
#include "Annotation.hpp"
#include "CodeModel.hpp"

namespace CodeListener::AnnotationServices
{

// inspired by https://llvm.org/doxygen/classllvm_1_1AnalysisManager.html
class AnalysisManager
{
  public:
    template <typename T>
    const T &getAnnotation(const Core::CodeModel &model)
    {
        AnalysisKey *key = &T::Key;
        auto it = cache.find(key);
        if (it == cache.end())
        {
            auto annotation = std::make_unique<T>(T::build(model));
            it = cache.emplace(key, std::move(annotation)).first;
        }

        return *static_cast<T *>(it->second.get());
    }

  private:
    std::unordered_map<AnalysisKey *, std::unique_ptr<Annotation>> cache;
};

} // namespace CodeListener::AnnotationServices
