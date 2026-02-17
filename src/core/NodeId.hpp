#pragma once

#include <cstdint>
#include <string>

namespace CodeListener
{

namespace Core
{

enum class NodeId : uint32_t
{
    INVALID = 0,
};

inline std::string toString(NodeId id)
{
    if (id == NodeId::INVALID)
    {
        return "NodeId(INVALID)";
    }
    return "NodeId(" + std::to_string(static_cast<uint32_t>(id)) + ")";
}

} // namespace Core

} // namespace CodeListener
