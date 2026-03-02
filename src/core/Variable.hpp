#pragma once

#include <string>
#include <vector>

#include "Linkage.hpp"
#include "NodeId.hpp"
#include "Operand.hpp"
#include "Scope.hpp"
#include "SourceLocation.hpp"
#include "StorageDuration.hpp"

namespace CodeListener
{

namespace Core
{

struct Variable
{
    NodeId id;
    std::string name;
    NodeId type_id;

    SourceLocation source_location;
    Scope scope;

    // C storage semantics
    StorageDuration storage_duration = StorageDuration::AUTO;
    Linkage linkage = Linkage::NONE;

    // bitfields in structs
    bool is_bitfield = false;
    int bitfield_size = 0;   // in bits
    int bitfield_offset = 0; // in bits from the start of the storage unit

    // true for variables created by the compiler for internal purposes
    bool artificial = false;

    // initializer for global/static variables
    std::vector<Operand> initial_value;
};

} // namespace Core

} // namespace CodeListener
