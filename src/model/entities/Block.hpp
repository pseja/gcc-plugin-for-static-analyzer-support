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

struct Block
{
    BlockId id;
    FunctionId parent;
    std::string name;

    // ast children
    std::vector<InstructionId> instruction_ids;

    // cfg edges
    std::vector<BlockId> predecessors;
    std::vector<BlockId> successors;
};

} // namespace CodeListener::Core
