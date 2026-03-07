#pragma once

#include <cstdint>
#include <ostream>
#include <string>

namespace CodeListener::Core
{

enum class NodeId : uint32_t
{
    INVALID = 0,
};

inline std::ostream &operator<<(std::ostream &os, NodeId id)
{
    os << static_cast<uint32_t>(id);
    return os;
}

inline std::string toString(NodeId id)
{
    if (id == NodeId::INVALID)
    {
        return "NodeId(INVALID)";
    }
    return "NodeId(" + std::to_string(static_cast<uint32_t>(id)) + ")";
}

} // namespace CodeListener::Core
