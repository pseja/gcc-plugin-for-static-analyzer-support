/**
 * @file IdMaps.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares identifier remapping tables used when merging serialized models.
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

#include <unordered_map>

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Exporters
{

// maps per-TU IDs to the corresponding merged-model IDs
struct IdMaps
{
    std::unordered_map<Core::TypeId, Core::TypeId> type_map;
    std::unordered_map<Core::VariableId, Core::VariableId> var_map;
    std::unordered_map<Core::FunctionId, Core::FunctionId> func_map;
    std::unordered_map<Core::BlockId, Core::BlockId> block_map;
    std::unordered_map<Core::InstructionId, Core::InstructionId> instr_map;
};

} // namespace CodeListener::Exporters
