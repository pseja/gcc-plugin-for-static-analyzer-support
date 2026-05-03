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

/** Represents one instruction stored in the CodeModel. */
struct Instruction
{
    /** Stable identifier of the instruction. */
    InstructionId id;

    /** Identifier of the block that owns the instruction. */
    BlockId parent_block_id;

    /** Cached coarse-grained instruction kind used for quick checks and debugging. */
    InstructionKind kind;

    /** Detailed payload describing the concrete instruction. */
    InstructionData data;

    /** Best-effort source location associated with the instruction. */
    SourceLocation source_location;

    /** Whether the instruction terminates its containing basic block. */
    bool is_terminator{false};
};

} // namespace CodeListener::Core
