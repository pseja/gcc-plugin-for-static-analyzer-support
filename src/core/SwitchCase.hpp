#pragma once

#include <optional>

#include "BlockId.hpp"
#include "Operand.hpp"

namespace CodeListener::Core
{

struct SwitchCase
{
    // if low_value is nullopt, this is the default case
    std::optional<Operand> low_value;
    // if high_value is present, this is a range case (low ... high)
    std::optional<Operand> high_value;

    BlockId target_block_id;

    SourceLocation source_location;
};

} // namespace CodeListener::Core
