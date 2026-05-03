/**
 * @file InstructionKind.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines instruction categories tracked by the CodeModel.
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

#include <string_view>

namespace CodeListener::Core
{

/** Enumerates instruction categories represented in the CodeModel. */
enum class InstructionKind
{
    ASSIGN, /**< Assignment or expression evaluation, for example `a = b`. */
    CALL,   /**< Function call. */
    RETURN, /**< Return from the current function. */
    COND,   /**< Conditional branch. */
    SWITCH, /**< Multi-way branch. */
    GOTO,   /**< Unconditional branch. */
    LABEL,  /**< Label definition. */
    ASM,    /**< Inline assembly statement. */

    // GIMPLE_BIND, // lexical scope with variables
    PHI, /**< SSA phi node at a control-flow merge point. */
    NOP, /**< No-operation placeholder. */
    // GIMPLE_DEBUG, // debug information
    // GIMPLE_PREDICT, // branch prediction hint
    CLOBBER,     /**< Variable lifetime end marker. */
    UNREACHABLE, /**< Control-flow dead end. */
    ABORT,       /**< Abort, trap, or unreachable terminator. */

    // GIMPLE_TRY, // __attribute__((cleanup))
    // GIMPLE_TRANSACTION, // __transaction_atomic

    UNKNOWN /**< Instruction without a dedicated representation. */
};

/**
 * Convert an instruction kind to its stable textual name.
 *
 * @param kind Instruction kind to stringify.
 *
 * @return Short symbolic name used in debugging and serialization.
 */
constexpr std::string_view toString(InstructionKind kind) noexcept
{
    switch (kind)
    {
    case InstructionKind::ASSIGN:
        return "ASSIGN";
    case InstructionKind::CALL:
        return "CALL";
    case InstructionKind::RETURN:
        return "RETURN";
    case InstructionKind::COND:
        return "COND";
    case InstructionKind::SWITCH:
        return "SWITCH";
    case InstructionKind::GOTO:
        return "GOTO";
    case InstructionKind::LABEL:
        return "LABEL";
    case InstructionKind::ASM:
        return "ASM";
    case InstructionKind::PHI:
        return "PHI";
    case InstructionKind::NOP:
        return "NOP";
    case InstructionKind::CLOBBER:
        return "CLOBBER";
    case InstructionKind::UNREACHABLE:
        return "UNREACHABLE";
    case InstructionKind::ABORT:
        return "ABORT";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
