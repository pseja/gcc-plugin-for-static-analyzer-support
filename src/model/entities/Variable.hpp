/**
 * @file Variable.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares variable entities stored in the CodeModel.
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

#include "SourceLocation.hpp"
#include "TypeId.hpp"
#include "VariableData.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

/** Represents one variable-like entity stored in the CodeModel. */
struct Variable
{
    /** Stable identifier of the variable. */
    VariableId id;

    /** Source or synthetic name of the variable. */
    std::string name{"<anonymous>"};

    /** Identifier of the variable's declared type. */
    TypeId type_id;

    /** Source location where the variable originates. */
    SourceLocation source_location;

    /** Whether the variable was synthesized by the compiler rather than written explicitly. */
    bool artificial{false};

    /** Detailed payload describing the concrete variable category. */
    VariableData data;
};

} // namespace CodeListener::Core
