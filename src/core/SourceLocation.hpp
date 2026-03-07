#pragma once

#include <string>

namespace CodeListener::Core
{

struct SourceLocation
{
    std::string file{"<unknown>"};
    std::string function{"<unknown>"};
    int line{0};
    int column{0};
    void *native_handle{nullptr};

    SourceLocation() = default;
    SourceLocation(std::string file, std::string func, int line, int column, void *native_handle = nullptr);
    SourceLocation(std::string file, int line, int column, void *native_handle = nullptr);
};

inline std::string toString(const SourceLocation &loc)
{
    return "SourceLocation(" + loc.file + ":" + loc.function + ":" + std::to_string(loc.line) + ":" +
           std::to_string(loc.column) + ")";
}

} // namespace CodeListener::Core
