#pragma once

#include <string>
#include <vector>

#include "NodeId.hpp"
#include "TypeKind.hpp"

namespace CodeListener
{

namespace Core
{

struct Type
{
    NodeId id;
    TypeKind kind;
    std::string name;
    int size; // in bytes
    // TODO: scope?
    // TODO: source_location?
    // TODO: array_size?
    // TODO: ptr_type (pointer, lvalue ref, rvalue ref)?

    bool is_const;
    bool is_unsigned;
    // TODO: is_volatile?

    std::vector<NodeId> nested_type_ids;
};

} // namespace Core

} // namespace CodeListener
