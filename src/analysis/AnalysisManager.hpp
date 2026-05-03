/**
 * @file AnalysisManager.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the lazy cache used to compute and share derived annotations.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <unordered_map>

#include "AnalysisKey.hpp"
#include "Annotation.hpp"
#include "CodeModel.hpp"

namespace CodeListener::AnnotationServices
{

/**
 * Lazily builds and caches derived annotations for a single CodeModel instance.
 *
 * Inspired by LLVM's `AnalysisManager` (https://llvm.org/doxygen/classllvm_1_1AnalysisManager.html)
 */
class AnalysisManager
{
  public:
    /**
     * Retrieve a cached annotation, building it on first use.
     *
     * @tparam T Concrete annotation type exposing `static AnalysisKey Key` and `static T build(const CodeModel&)`.
     * @param model Model used when the annotation needs to be computed.
     *
     * @return Cached annotation instance of type `T`.
     */
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
    /** Cache keyed by annotation type identity. */
    std::unordered_map<AnalysisKey *, std::unique_ptr<Annotation>> cache;
};

} // namespace CodeListener::AnnotationServices
