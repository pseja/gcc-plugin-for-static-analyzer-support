/**
 * @file AnnotationBase.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the polymorphic base type for cached annotations.
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

#include "AnalysisKey.hpp"
#include "Annotation.hpp"

namespace CodeListener::AnnotationServices
{

/**
 * CRTP base class used by concrete annotations to expose a unique cache key.
 *
 * @tparam Derived Concrete annotation type.
 */
template <typename Derived>
struct AnnotationBase : public Annotation
{
    /**
     * Unique key instance used to identify the derived annotation type in the cache.
     *
     * Each specialisation of AnnotationBase<Derived> gets its own statically allocated key
     * that is used to look up the corresponding annotation in the AnalysisContext cache.
     */
    static AnalysisKey Key;
};

/// @cond INTERNAL
template <typename Derived>
AnalysisKey AnnotationBase<Derived>::Key;
/// @endcond

} // namespace CodeListener::AnnotationServices
