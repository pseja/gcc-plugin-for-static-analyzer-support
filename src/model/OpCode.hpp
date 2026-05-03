/**
 * @file OpCode.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines operation codes used by assignment-like instructions.
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

/** Enumerates operation codes used by expression-like instructions. */
enum class OpCode
{
    NONE,          /**< Not applicable - for operations such as calls or returns. */
    NEGATE,        /**< Unary negation: `-x`. */
    BIT_NOT,       /**< Bitwise negation: `~x`. */
    LOG_NOT,       /**< Logical negation: `!x`. */
    ABS,           /**< Absolute value: `abs(x)`. */
    ADD,           /**< Addition: `x + y`. */
    SUB,           /**< Subtraction: `x - y`. */
    MUL,           /**< Multiplication: `x * y`. */
    DIV,           /**< Division: `x / y`. */
    MOD,           /**< Modulo: `x % y`. */
    BIT_AND,       /**< Bitwise AND: `x & y`. */
    BIT_OR,        /**< Bitwise OR: `x | y`. */
    BIT_XOR,       /**< Bitwise XOR: `x ^ y`. */
    SHL,           /**< Left shift: `x << y`. */
    SHR,           /**< Right shift: `x >> y`. */
    LOG_AND,       /**< Logical AND: `x && y`. */
    LOG_OR,        /**< Logical OR: `x || y`. */
    EQUAL,         /**< Equality comparison: `x == y`. */
    NOT_EQUAL,     /**< Inequality comparison: `x != y`. */
    GREATER_THAN,  /**< Greater-than comparison: `x > y`. */
    LESS_THAN,     /**< Less-than comparison: `x < y`. */
    GREATER_EQUAL, /**< Greater-or-equal comparison: `x >= y`. */
    LESS_EQUAL,    /**< Less-or-equal comparison: `x <= y`. */
    POINTER_ADD,   /**< Pointer addition: `ptr + offset`. */
    POINTER_SUB,   /**< Pointer subtraction or pointer difference: `ptr - offset`. */
    MIN,           /**< Minimum selection: `MIN(x, y)`. */
    MAX,           /**< Maximum selection: `MAX(x, y)`. */
    ROTATE_LEFT,   /**< Bitwise rotate left: `ROL(x, y)`. */
    ROTATE_RIGHT,  /**< Bitwise rotate right: `ROR(x, y)`. */
    CAST,          /**< Type conversion: `(cast)x`. */
};

/**
 * Convert an opcode to its stable textual name.
 *
 * @param op Opcode to stringify.
 *
 * @return Short symbolic name used in debugging and serialization.
 */
constexpr std::string_view toString(OpCode op) noexcept
{
    switch (op)
    {
    case OpCode::NONE:
        return "NONE";
    case OpCode::NEGATE:
        return "NEGATE";
    case OpCode::BIT_NOT:
        return "BIT_NOT";
    case OpCode::LOG_NOT:
        return "LOG_NOT";
    case OpCode::ABS:
        return "ABS";
    case OpCode::ADD:
        return "ADD";
    case OpCode::SUB:
        return "SUB";
    case OpCode::MUL:
        return "MUL";
    case OpCode::DIV:
        return "DIV";
    case OpCode::MOD:
        return "MOD";
    case OpCode::BIT_AND:
        return "BIT_AND";
    case OpCode::BIT_OR:
        return "BIT_OR";
    case OpCode::BIT_XOR:
        return "BIT_XOR";
    case OpCode::SHL:
        return "SHL";
    case OpCode::SHR:
        return "SHR";
    case OpCode::LOG_AND:
        return "LOG_AND";
    case OpCode::LOG_OR:
        return "LOG_OR";
    case OpCode::EQUAL:
        return "EQUAL";
    case OpCode::NOT_EQUAL:
        return "NOT_EQUAL";
    case OpCode::GREATER_THAN:
        return "GREATER_THAN";
    case OpCode::LESS_THAN:
        return "LESS_THAN";
    case OpCode::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case OpCode::LESS_EQUAL:
        return "LESS_EQUAL";
    case OpCode::POINTER_ADD:
        return "POINTER_ADD";
    case OpCode::POINTER_SUB:
        return "POINTER_SUB";
    case OpCode::MIN:
        return "MIN";
    case OpCode::MAX:
        return "MAX";
    case OpCode::ROTATE_LEFT:
        return "ROTATE_LEFT";
    case OpCode::ROTATE_RIGHT:
        return "ROTATE_RIGHT";
    case OpCode::CAST:
        return "CAST";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
