#pragma once

#include "TypeId.hpp"

namespace CodeListener::Core
{

struct PointerType
{
    TypeId pointee_type_id;
    bool is_restrict;
};

} // namespace CodeListener::Core
