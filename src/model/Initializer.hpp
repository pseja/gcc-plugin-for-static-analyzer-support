#pragma once

#include <memory>  // std::shared_ptr
#include <variant> // std::variant
#include <vector>  // std::vector

#include "Operand.hpp"

namespace CodeListener::Core
{

struct InitializerList;

using Initializer = std::variant<Operand, std::shared_ptr<InitializerList>>;

struct InitializerList
{
    std::vector<Initializer> elements;
};

} // namespace CodeListener::Core
