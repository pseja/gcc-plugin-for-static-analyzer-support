/**
 * @file JSONImporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the JSON deserializer for CodeModel instances.
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

#include <istream>
#include <string>

#include "CodeModel.hpp"

namespace CodeListener::Exporters
{

/**
 * Reconstructs a CodeModel from the JSON format.
 */
class JSONImporter
{
  public:
    /**
     * Load a model from a JSON file.
     *
     * @param filepath Path to the JSON document.
     * @return Deserialized CodeModel.
     */
    static Core::CodeModel importFromFile(const std::string &filepath);

    /**
     * Load a model from an existing input stream.
     *
     * @param is Input stream containing a JSON document.
     * @return Deserialized CodeModel.
     */
    static Core::CodeModel importFromStream(std::istream &is);
};

} // namespace CodeListener::Exporters
