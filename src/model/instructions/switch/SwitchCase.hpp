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

/** Describes one case arm or default arm of a switch instruction. */
struct SwitchCase
{
    /** Lower bound of the case value, or `std::nullopt` for the default case. */
    std::optional<Operand> low_value;

    /** Upper bound of the case range, present only for GCC range cases `low ... high`. */
    std::optional<Operand> high_value;

    /** Block entered when this case matches. */
    BlockId target_block_id;

    /** Source location of the case label in the original program. */
    SourceLocation source_location;
};

} // namespace CodeListener::Core
