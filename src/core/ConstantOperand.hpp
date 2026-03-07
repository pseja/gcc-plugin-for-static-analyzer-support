#pragma once

#include <string>

#include "NodeId.hpp"

namespace CodeListener::Core
{

struct ConstantOperand
{
    NodeId id;
    std::string value;
};

} // namespace CodeListener::Core
