#pragma once

#include <string_view>   // std::string_view
#include <string>        // std::string
#include <vector>        // std::vector
#include <unordered_map> // std::unordered_map
#include <memory>        // std::unique_ptr

#include "StringRef.hpp"

namespace CodeListener::Core
{

class Interner
{
  public:
    StringRef get(std::string_view string);
    std::string_view view(StringRef symbol) const;

  private:
    std::vector<std::unique_ptr<std::string>> string_pool; // NOTE: unique_ptr for string_view stability
    std::unordered_map<std::string_view, StringRef> interner;
};

} // namespace CodeListener::Core
