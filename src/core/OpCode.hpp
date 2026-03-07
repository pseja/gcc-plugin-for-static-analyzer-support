#pragma once

#include <string>

namespace CodeListener::Core
{

enum class OpCode
{
    NONE, // not applicable (calls, returns)
    // unary
    NEGATE,  // -x
    BIT_NOT, // ~x
    LOG_NOT, // !x
    ABS,     // abs(x)

    // binary
    ADD, // x + y
    SUB, // x - y
    MUL, // x * y
    DIV, // x / y
    MOD, // x % y

    // bitwise
    BIT_AND, // x & y
    BIT_OR,  // x | y
    BIT_XOR, // x ^ y
    SHL,     // x << y
    SHR,     // x >> y

    // logical
    LOG_AND, // x && y
    LOG_OR,  // x || y

    // comparisons
    EQUAL,         // x == y
    NOT_EQUAL,     // x != y
    GREATER_THAN,  // x > y
    LESS_THAN,     // x < y
    GREATER_EQUAL, // x >= y
    LESS_EQUAL,    // x <= y

    // memory
    ADDRESS_OF,  // &x
    POINTER_ADD, // ptr + offset
    POINTER_SUB, // ptr - ptr (diff)

    // numeric
    MIN, // min(x, y)
    MAX, // max(x, y)

    // bitwise rotation
    ROTATE_LEFT,  // rotate x left by y
    ROTATE_RIGHT, // rotate x right by y

    // type conversion
    CAST, // (type)x
};

inline std::string toString(OpCode op)
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
    case OpCode::ADDRESS_OF:
        return "ADDRESS_OF";
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
