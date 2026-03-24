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
    // FIXME: this isn't being assigned everywhere it should be, so it is not fully reliable
    bool artificial{false};

    VariableData data;
};

} // namespace CodeListener::Core
