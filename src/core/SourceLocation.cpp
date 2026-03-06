#include <string>

#include "SourceLocation.hpp"

namespace CodeListener
{

namespace Core
{

SourceLocation::SourceLocation(std::string file, std::string func, int line, int column, void *native_handle)
    : file(std::move(file)), function(std::move(func)), line(line), column(column), native_handle(native_handle)
{
}
SourceLocation::SourceLocation(std::string file, int line, int column, void *native_handle)
    : file(std::move(file)), line(line), column(column), native_handle(native_handle)
{
}

} // namespace Core

} // namespace CodeListener
