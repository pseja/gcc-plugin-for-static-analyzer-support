#pragma once

#include <string>
#include <string_view>
#include <sstream>

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

inline std::string_view toString(const SourceLocation &loc)
{
    std::ostringstream oss;
    oss << loc.file << ':' << loc.function << ':' << loc.line << ':' << loc.column;
    return oss.str();
}

} // namespace CodeListener::Core
