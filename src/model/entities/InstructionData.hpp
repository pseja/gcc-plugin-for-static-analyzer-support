/**
 * @file InstructionData.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares instruction payload variants stored by Instruction.
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

#include <variant>

#include "AbortInstruction.hpp"
#include "AsmInstruction.hpp"
#include "AssignInstruction.hpp"
#include "CallInstruction.hpp"
#include "ClobberInstruction.hpp"
#include "CondInstruction.hpp"
#include "GotoInstruction.hpp"
#include "LabelInstruction.hpp"
#include "PhiInstruction.hpp"
#include "ReturnInstruction.hpp"
#include "SwitchInstruction.hpp"
#include "UnknownInstruction.hpp"
#include "UnreachableInstruction.hpp"

namespace CodeListener::Core
{

/** Variant containing the concrete payload stored by one `Instruction` entity. */
using InstructionData =
    std::variant<std::monostate, AssignInstruction, CallInstruction, GotoInstruction, LabelInstruction, AsmInstruction,
                 ReturnInstruction, CondInstruction, SwitchInstruction, PhiInstruction, ClobberInstruction,
                 UnreachableInstruction, AbortInstruction, UnknownInstruction>;

} // namespace CodeListener::Core
