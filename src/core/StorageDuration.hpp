#pragma once

#include <string>

namespace CodeListener::Core
{

enum class StorageDuration
{
    AUTO,
    STATIC,
    EXTERN,
    REGISTER,
    THREAD_LOCAL,
};

inline std::string toString(StorageDuration storage_duration)
{
    switch (storage_duration)
    {
    case StorageDuration::AUTO:
        return "AUTO";
    case StorageDuration::STATIC:
        return "STATIC";
    case StorageDuration::EXTERN:
        return "EXTERN";
    case StorageDuration::REGISTER:
        return "REGISTER";
    case StorageDuration::THREAD_LOCAL:
        return "THREAD_LOCAL";
    default:
        return "UNKNOWN";
    }
}

} // namespace CodeListener::Core
