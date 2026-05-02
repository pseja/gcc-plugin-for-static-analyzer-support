#pragma once

#include <string>

#include "TypeId.hpp"
#include "TypeKind.hpp"
#include "TypeData.hpp"

namespace CodeListener::Core
{

struct Type
{
    TypeId id;
    std::string name; // "int", "struct Foo", ...

    // qualifiers and attributes
    bool is_const{false};
    bool is_volatile{false};
    // bool is_restrict{false}; // only for pointers
    bool is_atomic{false}; // _Atomic

    // size and memory layout
    int size_bits; // for bitfields
    // int size_bytes; // sizeof
    int alignment; // in bytes

    // FIXME: not needed anymore thanks to the TypeData variant, but keeping it for easier querying and debugging
    // for now
    TypeKind kind;
    TypeData data;
};

} // namespace CodeListener::Core
