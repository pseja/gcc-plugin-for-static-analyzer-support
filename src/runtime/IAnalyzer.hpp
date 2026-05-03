/**
 * @file IAnalyzer.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the runtime interface implemented by analyzer bridges.
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

#include "AnalysisContext.hpp"
#include "CodeModel.hpp"

namespace CodeListener::Core
{

/**
 * Abstract interface implemented by every analyzer backend.
 *
 * Concrete implementations are:
 *   - NativeAnalyzerBridge - wraps cl_native_analyzer_api_t (C-ABI shared lib)
 *   - LegacyPredatorBridge - wraps cl_code_listener (old Predator C API)
 */
class IAnalyzer
{
  public:
    /** Virtual destructor for polymorphic analyzer ownership. */
    virtual ~IAnalyzer() = default;

    /**
     * Run the analysis on the fully-populated model.
     *
     * @param model The CodeModel to analyze.
     * @param ctx   Services: GCC-formatted reporter, shared annotation cache, and export helpers.
     *
     * @return true if analysis succeeded; false if at least one error was found
     */
    virtual bool analyze(const CodeModel &model, AnalysisContext &ctx) = 0;
};

} // namespace CodeListener::Core
