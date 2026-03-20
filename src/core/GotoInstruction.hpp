#pragma once

#include "BlockId.hpp"

namespace CodeListener::Core
{

struct GotoInstruction
{
    // FIXME: maybe `Operand destination`
    BlockId target;
};

} // namespace CodeListener::Core
