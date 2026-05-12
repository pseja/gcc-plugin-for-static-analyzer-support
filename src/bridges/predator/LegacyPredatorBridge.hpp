/**
 * @file LegacyPredatorBridge.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the bridge that adapts the legacy Predator C API to IAnalyzer.
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

#include "IAnalyzer.hpp"

struct cl_code_listener;

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Bridges the legacy cl_code_listener C API (Predator's old interface) to IAnalyzer.
 */
class LegacyPredatorBridge : public Core::IAnalyzer
{
  public:
    /**
     * Constructs a bridge over an already created legacy listener instance.
     *
     * @param listener Legacy analyzer instance created through the C API.
     */
    explicit LegacyPredatorBridge(struct cl_code_listener *listener);

    /** @copydoc Core::IAnalyzer::analyze */
    bool analyze(const Core::CodeModel &model, AnalysisContext &ctx) override;

  private:
    /** Borrowed legacy listener instance receiving replayed callbacks. */
    struct cl_code_listener *listener;
};

} // namespace CodeListener::CompilerAbstractionLayer
