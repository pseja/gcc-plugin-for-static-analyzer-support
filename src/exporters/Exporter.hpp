/**
 * @file Exporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the common visitor-based base class for model exporters.
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

    void exportModel(const Core::CodeModel &model);

  protected:
    AnnotationServices::AnalysisManager analysis_manager;
};

} // namespace CodeListener::Exporters
