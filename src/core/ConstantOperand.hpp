#pragma once

#include <string>

#include "NodeId.hpp"

namespace CodeListener
{

namespace Core
{

struct ConstantOperand
{
    NodeId id;
    std::string value;
};

} // namespace Core

} // namespace CodeListener