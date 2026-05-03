/**
 * @file Function.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares function entities stored in the CodeModel.
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

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "SourceLocation.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

/** Represents one function stored in the CodeModel. */
struct Function
{
    /** Stable identifier of the function. */
    FunctionId id;

    /** Function name as reported by the frontend. */
    std::string name;

    /** Identifier of the function return type. */
    TypeId return_type_id;

    /** Source location associated with the function definition or declaration. */
    SourceLocation source_location;

    /** Ordered identifiers of formal parameters. */
    std::vector<VariableId> parameter_ids;

    /** Identifiers of local variables owned by the function body. */
    std::vector<VariableId> local_variable_ids;

    /** Ordered identifiers of blocks that form the function body. */
    std::vector<BlockId> block_ids;
};

} // namespace CodeListener::Core
