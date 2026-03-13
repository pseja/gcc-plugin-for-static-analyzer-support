#pragma once

#include <string>
#include <vector>

#include "TypeId.hpp"
#include "TypeKind.hpp"

namespace CodeListener::Core
{

struct Type
{
    TypeId id;
    TypeKind kind;
    std::string name; // "int", "struct Foo", ...

    // size and memory layout
    int size_bits;  // for bitfields
    // TODO: this can easily be derived from size_bits, and having both may lead to inconsistency
    int size_bytes; // sizeof
    int alignment;  // in bytes

    // qualifiers and attributes
    bool is_const = false;
    bool is_volatile = false;
    bool is_restrict = false;
    bool is_atomic = false;

    // type specifics
    bool is_unsigned = false;
    bool is_struct = false;
    bool is_union = false;

    // number of elements for arrays (0 for VLA or flexible array member)
    int array_element_count = 0;

    // structure
    // - for ptr/array: [0] is the pointee/element type
    // - for struct/union: list of field declarations (variables)
    // - for function: [0] return type, rest are parameter types
    std::vector<TypeId> nested_type_ids;
};

} // namespace CodeListener::Core
