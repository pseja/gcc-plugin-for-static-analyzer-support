/**
 * @file NativeAnalyzerBridge.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the bridge that adapts the native analyzer API (C-ABI) to IAnalyzer.
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

#include <string>

#include <cl_native_analyzer_api.h>

#include "IAnalyzer.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Bridges cl_native_analyzer_api_t (C ABI, loaded via dlopen) to IAnalyzer.
 */
class NativeAnalyzerBridge : public Core::IAnalyzer
{
  public:
    /**
     * Constructs a bridge over one native analyzer API table.
     *
     * @param api Native analyzer API table (C-ABI) exported by the loaded shared library.
     * @param args Opaque argument string forwarded to the analyzer.
     */
    explicit NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args = {});

    /** @copydoc Core::IAnalyzer::analyze */
    bool analyze(const Core::CodeModel &model, AnalysisContext &ctx) override;

  private:
    /** Borrowed native analyzer API table returned by the loaded shared library. */
    const cl_native_analyzer_api_t *api;

    /** Opaque argument string forwarded to the analyzer on each run. */
    std::string args;
};

} // namespace CodeListener::CompilerAbstractionLayer
