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

enum class InstructionKind
{
    // core C statements
    ASSIGN, // a = b
    CALL,   // f()
    RETURN, // return val
    COND,   // if (cond) ...
    SWITCH, // switch (val) ...
    GOTO,   // goto
    LABEL,  // label:
    ASM,    // asm(...)

    // structure and scope
    // GIMPLE_BIND, // lexical scope with variables
    PHI, // phi node (if SSA) - SSA merge point
    NOP, // nop
    // GIMPLE_DEBUG, // debug information
    // GIMPLE_PREDICT, // branch prediction hint
    CLOBBER,     // variable lifetime end
    UNREACHABLE, // control flow dead end
    ABORT,       // abort/trap/unreachable

    // C extensions (GCC specific)
    // GIMPLE_TRY, // __attribute__((cleanup))
    // GIMPLE_TRANSACTION, // __transaction_atomic

    UNKNOWN
};

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
