#pragma once

#include <vector>

#include "TypeId.hpp"

namespace CodeListener::Core
{

struct FunctionType
{
    TypeId return_type_id;
    std::vector<TypeId> parameter_type_ids;
    bool is_variadic{false};
};

} // namespace CodeListener::Core
