/**
 * @file JSONFrontend.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the offline frontend that loads a CodeModel from JSON.
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
#include <vector>
#include <memory>

#include "IAnalyzer.hpp"
#include "IFrontend.hpp"
#include "JSONImporter.hpp"

namespace CodeListener::Exporters
{

/**
 * Frontend that loads an already-serialised CodeModel from a JSON file.
 *
 * Used by the cl_analyze standalone tool. The file is loaded once in run(), then every analyzer in the provided list is
 * invoked against the model.
 */
class JSONFrontend : public Core::IFrontend
{
  public:
    explicit JSONFrontend(std::string json_path);

    bool run(std::vector<std::unique_ptr<Core::IAnalyzer>> &analyzers, CodeListener::AnalysisContext &ctx) override;

  private:
    std::string json_path;
};

} // namespace CodeListener::Exporters
