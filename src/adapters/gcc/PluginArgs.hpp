#pragma once

#include <gcc-plugin.h> // plugin_name_args
#include <string>       // string
#include <vector>       // vector, pair

namespace CodeListener
{

namespace GCC_ADAPTER
{

struct PluginArgs
{
    std::string base_name;
    std::string full_name;
    std::vector<std::pair<std::string, std::string>> args;

    PluginArgs(const plugin_name_args *plugin_info);

    void print() const;
};

} // namespace GCC_ADAPTER

} // namespace CodeListener
