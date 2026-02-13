#pragma once

#include <string>

namespace CodeListener
{

namespace Core
{

struct SourceLocation
{
    std::string file;
    std::string function;
    int line;
    int column;
    void *native_handle = nullptr;

    SourceLocation() : line(0), column(0), native_handle(nullptr)
    {
    }
    SourceLocation(std::string file, std::string func, int line, int column, void *native_handle = nullptr)
        : file(std::move(file)), function(std::move(func)), line(line), column(column), native_handle(native_handle)
    {
    }
    SourceLocation(std::string file, int line, int column, void *native_handle = nullptr)
        : file(std::move(file)), line(line), column(column), native_handle(native_handle)
    {
    }
};

} // namespace Core

} // namespace CodeListener
