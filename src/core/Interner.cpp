#pragma once

#include <string_view> // std::string_view

#include "Interner.hpp"
#include "StringRef.hpp"

namespace CodeListener::Core
{

StringRef Interner::get(std::string_view string)
{
    auto it = interner.find(string);
    if (it != interner.end())
    {
        return it->second;
    }

    std::unique_ptr<std::string> &kept_string = string_pool.emplace_back(std::make_unique<std::string>(string));
    StringRef new_string_ref{static_cast<uint32_t>(string_pool.size() - 1)};
    interner[*kept_string] = new_string_ref;

    return new_string_ref;
}

std::string_view Interner::view(StringRef symbol) const
{
    if (symbol.id >= string_pool.size())
    {
        return "";
    }

    return *string_pool[symbol.id];
}

} // namespace CodeListener::Core
