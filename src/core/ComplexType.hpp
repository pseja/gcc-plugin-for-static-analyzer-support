#pragma once

#include "TypeId.hpp"

namespace CodeListener::Core
{

// C99 _Complex types
struct ComplexType
{
    // the underlying real/imaginary component type
    TypeId component_type_id;
};

} // namespace CodeListener::Core
