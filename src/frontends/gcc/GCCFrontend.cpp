/**
 * @file GCCFrontend.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the live frontend that runs analyzers over GCC-produced models.
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

#include "GCCFrontend.hpp"

#include "CodeModel.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

GCCFrontend::GCCFrontend(const Core::CodeModel &model) : model(model)
{
}

bool GCCFrontend::run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx)
{
    bool all_succeeded = true;
    for (auto &analyzer : analyzers)
    {
        if (!analyzer->analyze(model, ctx))
        {
            all_succeeded = false;
        }
    }
    return all_succeeded;
}

} // namespace CodeListener::CompilerAbstractionLayer
