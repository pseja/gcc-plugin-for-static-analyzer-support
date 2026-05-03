/**
 * @file JSONExporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the JSON serializer for CodeModel instances.
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

#include <fstream>
#include <iostream>
#include <string>

#include "Exporter.hpp"

namespace CodeListener::Exporters
{

/**
 * Serializes a CodeModel to the JSON format.
 */
class JSONExporter : public Exporter
{
  public:
    /**
     * Constructs an exporter that writes to an already opened stream.
     *
     * @param os Output stream receiving the serialized JSON document.
     */
    explicit JSONExporter(std::ostream &os);

    /**
     * Constructs an exporter that writes to a file.
     *
     * @param filepath Output file path opened by the exporter.
     */
    explicit JSONExporter(const std::string &filepath);

  protected:
    /**
     * Flush the complete JSON document once the visitor finished traversing the model.
     *
     * @param model Fully traversed model ready for serialization.
     */
    void onEndModel(const Core::CodeModel &model) override;

  private:
    /** Owned file stream used when the exporter was constructed from a path. */
    std::ofstream file_os;

    /** Effective output stream used by the exporter regardless of construction mode. */
    std::ostream &os;
};

} // namespace CodeListener::Exporters
