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

/** Maps per-translation-unit identifiers to their corresponding identifiers in the merged model. */
struct IdMaps
{
    /** Mapping from source type ids to merged-model type ids. */
    std::unordered_map<Core::TypeId, Core::TypeId> type_map;

    /** Mapping from source variable ids to merged-model variable ids. */
    std::unordered_map<Core::VariableId, Core::VariableId> var_map;

    /** Mapping from source function ids to merged-model function ids. */
    std::unordered_map<Core::FunctionId, Core::FunctionId> func_map;

    /** Mapping from source block ids to merged-model block ids. */
    std::unordered_map<Core::BlockId, Core::BlockId> block_map;

    /** Mapping from source instruction ids to merged-model instruction ids. */
    std::unordered_map<Core::InstructionId, Core::InstructionId> instr_map;
};

} // namespace CodeListener::Exporters
