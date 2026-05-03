/**
 * @file CallGraphEdge.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares edges stored in the call graph annotation.
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

#include <optional>

#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::AnnotationServices
{

/** Represents one call edge in the derived call graph. */
struct CallGraphEdge
{
    /** Identifier of the call instruction that produced this edge. */
    Core::InstructionId call_instruction;

    /** Target function identifier, or `std::nullopt` for indirect calls through function pointers. */
    std::optional<Core::FunctionId> callee;
};

} // namespace CodeListener::AnnotationServices
