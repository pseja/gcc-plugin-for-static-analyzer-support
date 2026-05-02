#pragma once

#include <optional>

#include "TypeId.hpp"

namespace CodeListener::Core
{

struct ArrayType
{
    TypeId element_type_id;
    std::optional<unsigned long> element_count; // nullopt for VLA or flexible array member
};

} // namespace CodeListener::Core
