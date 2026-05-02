/**
 * @file SwitchCase.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares case entries stored by switch instructions.
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

#include "BlockId.hpp"
#include "Operand.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::Core
{

struct SwitchCase
{
    // if low_value is nullopt, this is the default case
    std::optional<Operand> low_value;
    // if high_value is present, this is a range case (low ... high)
    std::optional<Operand> high_value;

    BlockId target_block_id;

    SourceLocation source_location;
};

} // namespace CodeListener::Core
