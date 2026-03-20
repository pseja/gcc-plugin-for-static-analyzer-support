#pragma once

#include "BlockId.hpp"
#include "Operand.hpp"

namespace CodeListener::Core
{

struct PhiIncomingValue
{
    BlockId block_id;
    Operand value;
};

} // namespace CodeListener::Core
