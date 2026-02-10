#include <iostream> // std::cout

#include "PluginContext.hpp"

namespace CodeListener
{

namespace GCC_ADAPTER
{

// TODO: plugin arguments?
struct plugin_info PluginContext::plugin_info = {
    .version = "0.1",
    .help = "A plugin to support static analyzers like Predator",
};

PluginContext &PluginContext::getInstance()
{
    static PluginContext instance;
    return instance;
}

void PluginContext::initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version)
{
    args = std::make_unique<PluginArgs>(plugin_info);

    init_print(version);

    // metadata
    register_callback(plugin_info->base_name, PLUGIN_INFO, nullptr, &PluginContext::plugin_info);

    // TODO: register callbacks

    // cleanup
    register_callback(plugin_info->base_name, PLUGIN_FINISH, on_plugin_finish, this);
}

void PluginContext::init_print(const plugin_gcc_version *version)
{
    std::cout << "Initializing Code Listener GCC plugin\n";
    std::cout << "GCC version: " << version->basever << "\n";

    if (args)
    {
        args->print();
    }
}

void PluginContext::on_plugin_finish(void *gcc_data, void *user_data)
{
    (void)gcc_data;
    (void)user_data;

    std::cout << "Code Listener GCC plugin finished\n";
}

} // namespace GCC_ADAPTER

} // namespace CodeListener
