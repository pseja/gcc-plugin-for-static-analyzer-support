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

struct CallGraphEdge
{
    Core::InstructionId call_instruction;
    // target function ID
    // - if nullopt, the call is an indirect call (function pointer)
    std::optional<Core::FunctionId> callee;
};

} // namespace CodeListener::AnnotationServices
