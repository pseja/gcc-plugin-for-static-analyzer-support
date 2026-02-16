#pragma once

#include <string>

#include "NodeId.hpp"
#include "Scope.hpp"
#include "SourceLocation.hpp"

namespace CodeListener
{

namespace Core
{

struct Variable
{
    NodeId id;
    std::string name;
    NodeId type_id;
    SourceLocation source_location;
    Scope scope;
};

} // namespace Core

} // namespace CodeListener
