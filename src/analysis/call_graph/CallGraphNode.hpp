/**
 * @file CallGraphNode.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares nodes stored in the call graph annotation.
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

#include <vector>

#include "CallGraphEdge.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::AnnotationServices
{

/** Represents one function node inside the derived call graph. */
struct CallGraphNode
{
    /** Identifier of the represented function. */
    Core::FunctionId function_id;

    /** Outgoing calls performed by this function. */
    std::vector<CallGraphEdge> outgoing_calls;

    /** Incoming calls targeting this function. */
    std::vector<CallGraphEdge> incoming_calls;

    /** Instructions where the address of this function was taken. */
    std::vector<Core::InstructionId> address_taken_at;
};

} // namespace CodeListener::AnnotationServices
