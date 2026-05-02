/**
 * @file AnalysisContext.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements export helpers exposed through AnalysisContext.
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

#include "AnalysisContext.hpp"

#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "PPExporter.hpp"

namespace CodeListener
{

void AnalysisContext::exportDot(const Core::CodeModel &model, const std::string &path,
                                Exporters::DotVerbosity verbosity)
{
    Exporters::DOTExporter exporter(path, verbosity);
    exporter.exportModel(model);
}

void AnalysisContext::exportPP(const Core::CodeModel &model, const std::string &path)
{
    Exporters::PPExporter exporter(path);
    exporter.exportModel(model);
}

void AnalysisContext::exportJson(const Core::CodeModel &model, const std::string &path)
{
    Exporters::JSONExporter exporter(path);
    exporter.exportModel(model);
}

} // namespace CodeListener
