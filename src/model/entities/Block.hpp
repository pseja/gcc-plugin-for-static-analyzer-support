/**
 * @file Block.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares basic block entities stored in the CodeModel.
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

#include <string> // std::string
#include <vector> // std::vector

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"

namespace CodeListener::Core
{

/** Represents one basic block stored in the CodeModel. */
struct Block
{
    /** Stable identifier of the block. */
    BlockId id;

    /** Identifier of the parent function containing the block. */
    FunctionId parent;

    /** Printed block label used by exporters and diagnostics. */
    std::string name;

    /** Ordered identifiers of instructions contained in the block. */
    std::vector<InstructionId> instruction_ids;

    /** Incoming CFG edges represented by predecessor block identifiers. */
    std::vector<BlockId> predecessors;

    /** Outgoing CFG edges represented by successor block identifiers. */
    std::vector<BlockId> successors;
};

} // namespace CodeListener::Core
