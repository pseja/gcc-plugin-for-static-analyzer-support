#pragma once

namespace CodeListener::Core
{

struct AbortInstruction
{
    // triggered by __builtin_abort, __builtin_trap, or __builtin_unreachable (with some compiler flags)
};

} // namespace CodeListener::Core
