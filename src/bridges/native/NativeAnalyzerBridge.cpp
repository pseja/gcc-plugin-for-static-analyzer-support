/**
 * @file NativeAnalyzerBridge.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the bridge that adapts the native analyzer API (C-ABI) to IAnalyzer.
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

#include "NativeAnalyzerBridge.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

NativeAnalyzerBridge::NativeAnalyzerBridge(const cl_native_analyzer_api_t *api, std::string args)
    : api(api), args(std::move(args))
{
}

bool NativeAnalyzerBridge::analyze(const Core::CodeModel &model, AnalysisContext &ctx)
{
    if (api && api->analyze)
    {
        return api->analyze(model, ctx, args.empty() ? nullptr : args.c_str());
    }
    return true;
}

} // namespace CodeListener::CompilerAbstractionLayer
