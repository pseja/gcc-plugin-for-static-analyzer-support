#include <iostream>     // std::cout

#include <gcc-plugin.h> // plugin_name_args

#include "PluginArgs.hpp"

namespace CodeListener
{

namespace GCC_ADAPTER
{

PluginArgs::PluginArgs(const plugin_name_args *plugin_info)
    : base_name(plugin_info->base_name), full_name(plugin_info->full_name)
{
    for (int i = 0; i < plugin_info->argc; i++)
    {
        args.emplace_back(plugin_info->argv[i].key, plugin_info->argv[i].value ? plugin_info->argv[i].value : "");
    }
}

void PluginArgs::print() const
{
    std::cout << "PluginArgs:\n";
    std::cout << "  Base name: '" << base_name << "'\n";
    std::cout << "  Full name: '" << full_name << "'\n";
    std::cout << "  Arguments:\n";
    for (const auto &[key, value] : args)
    {
        std::cout << "    Key: '" << key << "', Value: '" << value << "'\n";
    }
}

} // namespace GCC_ADAPTER

} // namespace CodeListener
