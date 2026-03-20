#pragma once

#include <string>

#include "SourceLocation.hpp"
#include "TypeId.hpp"
#include "VariableData.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

struct Variable
{
    VariableId id;
    std::string name{"<anonymous>"};
    TypeId type_id;

    SourceLocation source_location;

    // true for variables created by the compiler for internal purposes
    bool artificial{false};

    VariableData data;
};

} // namespace CodeListener::Core
