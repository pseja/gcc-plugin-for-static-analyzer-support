#pragma once

#include "BlockId.hpp"

namespace CodeListener::Core
{

struct GotoInstruction
{
    BlockId target;
};

} // namespace CodeListener::Core
