#include "StringRef.hpp"

namespace CodeListener::Core
{

bool StringRef::operator==(const StringRef &other) const
{
    return id == other.id;
}

} // namespace CodeListener::Core
