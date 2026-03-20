#pragma once

#include <optional>

namespace CodeListener::Core
{

struct FieldVariable
{
    std::optional<unsigned> bitfield_size;   // in bits
    std::optional<unsigned> bitfield_offset; // in bits from the start of the storage unit
};

} // namespace CodeListener::Core
