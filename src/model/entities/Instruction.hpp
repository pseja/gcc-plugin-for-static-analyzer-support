/**
 * @file Instruction.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares instruction entities stored in the CodeModel.
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

#include "BlockId.hpp"
#include "InstructionData.hpp"
#include "InstructionId.hpp"
#include "InstructionKind.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

struct Instruction
{
    InstructionId id;
    BlockId parent_block_id;

    // FIXME: not needed anymore thanks to the InstructionData variant, but keeping it for easier querying and debugging
    // for now
    InstructionKind kind;
    InstructionData data;

    SourceLocation source_location;

    // ends a basic block (e.g. GOTO, RET, SWITCH, COND)
    bool is_terminator{false};
};

} // namespace CodeListener::Core
