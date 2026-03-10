#pragma once

#include <string_view> // std::string_view

#include "Symbol.hpp"
#include "Context.hpp"

namespace CodeListener::Core
{

Symbol Context::get(std::string_view string)
{
    auto it = interner.find(string);
    if (it != interner.end())
    {
        return it->second;
    }

    std::unique_ptr<std::string> &kept_string = string_pool.emplace_back(std::make_unique<std::string>(string));
    Symbol new_symbol{static_cast<uint32_t>(string_pool.size() - 1)};
    interner[*kept_string] = new_symbol;

    return new_symbol;
}

std::string_view Context::view(Symbol symbol) const
{
    if (symbol.id >= string_pool.size())
    {
        return "";
    }

    return *string_pool[symbol.id];
}

} // namespace CodeListener::Core
