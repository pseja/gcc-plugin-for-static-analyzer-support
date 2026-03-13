#pragma once

#include <cstdint>
#include <string>

#include "TypeId.hpp"

namespace CodeListener::Core
{

struct FieldDeclaration
{
    std::string name;
    TypeId type_id;
    uint32_t offset;

    // only for bit fields
    uint32_t bit_offset{0}; // >0 for bit field
    uint32_t bit_size{0};   // >0 for bit field
};

} // namespace CodeListener::Core
