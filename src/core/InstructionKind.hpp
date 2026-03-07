
#pragma once

#include <string>

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

inline std::string toString(InstructionKind kind)
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
