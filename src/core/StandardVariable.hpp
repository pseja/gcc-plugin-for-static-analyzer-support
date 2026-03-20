#pragma once

#include <optional>

#include "Initializer.hpp"
#include "Linkage.hpp"
#include "Scope.hpp"
#include "StorageDuration.hpp"

namespace CodeListener::Core
{

struct StandardVariable
{
    Scope scope;
    StorageDuration storage_duration;
    Linkage linkage;
    std::optional<Initializer> initial_value;
};

} // namespace CodeListener::Core
