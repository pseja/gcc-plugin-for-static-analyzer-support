/**
 * @file GCCFrontend.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the live frontend that runs analyzers over GCC-produced models.
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

#include <vector>
#include <memory>

#include "AnalysisContext.hpp"
#include "IAnalyzer.hpp"
#include "IFrontend.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Frontend that receives a fully-processed CodeModel from the GCC pass manager (produced by GCCAdapter) and runs all
 * registered analyzers against it.
 */
class GCCFrontend : public Core::IFrontend
{
  public:
    /**
     * @param model The CodeModel populated by GCCAdapter during the GCC pass.
     */
    explicit GCCFrontend(const Core::CodeModel &model);

    /** @copydoc Core::IFrontend::run */
    bool run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx) override;

  private:
    /** Completed model produced by the GCC pass pipeline. */
    const Core::CodeModel &model;
};

} // namespace CodeListener::CompilerAbstractionLayer
