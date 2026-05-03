/**
 * @file AsmInstruction.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares IR payloads for inline assembly statements.
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

#include <string>
#include <vector>

#include "AsmOperandConstraint.hpp"

namespace CodeListener::Core
{

/** Payload representing one inline assembly statement. */
struct AsmInstruction
{
    /** Raw assembly template string. */
    std::string assembly_string;

    /** Input operands consumed by the assembly block. */
    std::vector<AsmOperandConstraint> inputs;

    /** Output operands produced by the assembly block. */
    std::vector<AsmOperandConstraint> outputs;

    /** Clobbered registers or special resources declared by the assembly block. */
    std::vector<std::string> clobbers;

    /** Whether the statement is marked `volatile` and must not be optimized away. */
    bool is_volatile{false};
};

} // namespace CodeListener::Core
