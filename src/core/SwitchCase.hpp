#pragma once

#include <optional>

#include "NodeId.hpp"
#include "Operand.hpp"

namespace CodeListener
{

namespace Core
{

struct SwitchCase
{
    // if low_value is nullopt, this is the default case
    std::optional<Operand> low_value;
    // if high_value is present, this is a range case (low ... high)
    std::optional<Operand> high_value;

    NodeId target_block_id = NodeId::INVALID;
};

} // namespace Core

} // namespace CodeListener
