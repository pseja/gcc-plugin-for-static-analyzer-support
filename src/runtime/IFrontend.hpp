/**
 * @file IFrontend.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the runtime interface implemented by model frontends.
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
#include <vector>

#include "AnalysisContext.hpp"
#include "IAnalyzer.hpp"

namespace CodeListener::Core
{

/**
 * Abstract interface for compiler/tool frontends.
 *
 * A frontend is responsible for populating a CodeModel and driving one full analysis pass over it.
 *
 * Concrete implementations include:
 *   - GCCFrontend  - receives GIMPLE from the GCC pass manager (cl_gcc plugin)
 *   - JSONFrontend - loads a pre-built CodeModel from a JSON file (cl_analyze)
 *
 * Inspired by LLVM's FrontendAction/ASTConsumer contract.
 */
class IFrontend
{
  public:
    virtual ~IFrontend() = default;

    /**
     * Run all registered analyzers on the model produced by this frontend.
     *
     * @param analyzers Ordered list of analyzer backends to invoke.
     * @param ctx       Shared analysis services (reporter, annotation cache, export helpers).
     *
     * @return true if every analyzer succeeded; false if any reported an error
     */
    virtual bool run(std::vector<std::unique_ptr<IAnalyzer>> &analyzers, AnalysisContext &ctx) = 0;
};

} // namespace CodeListener::Core
