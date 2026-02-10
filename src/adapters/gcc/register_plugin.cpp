#include <gcc-plugin.h>     // plugin_init, plugin_is_GPL_compatible
#include <iostream>         // std::cout
#include <plugin-version.h> // gcc_version

#include "PluginContext.hpp"
#include "register_plugin.hpp"

// required by GCC to indicate that the plugin is GPL compatible
int plugin_is_GPL_compatible;

void print_info(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    std::cout << "--- plugin info ---\n";
    std::cout << "plugin name: " << plugin_info->base_name << "\n";
    std::cout << "full plugin name: " << plugin_info->full_name << "\n";
    std::cout << "argument count: " << plugin_info->argc << "\n";
    for (int i = 0; i < plugin_info->argc; ++i)
    {
        std::cout << "arg " << i << ": key=" << plugin_info->argv[i].key << ", value=" << plugin_info->argv[i].value
                  << "\n";
    }
    // std::cout << "plugin version: " << plugin_info->version << "\n";
    // std::cout << "plugin help: " << plugin_info->help << "\n";

    std::cout << "\n--- GCC version info ---\n";
    std::cout << "basever: " << version->basever << "\n";
    std::cout << "datestamp: " << version->datestamp << "\n";
    std::cout << "devphase: " << version->devphase << "\n";
    std::cout << "revision: " << version->revision << "\n";
    std::cout << "configuration arguments: " << version->configuration_arguments << "\n";
}

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    // print_info(plugin_info, version);

    // FIXME: old cl had less strict version check for predator
    if (!plugin_default_version_check(version, &gcc_version))
    {
        std::cerr << "Incompatible GCC version: This plugin was compiled for version " << GCCPLUGIN_VERSION_MAJOR << "."
                  << GCCPLUGIN_VERSION_MINOR << ", but the current GCC version is " << version->basever << "\n";

        return 1;
    }

    CodeListener::GCC_ADAPTER::PluginContext &context = CodeListener::GCC_ADAPTER::PluginContext::getInstance();
    context.initialize(plugin_info, version);

    return 0;
}
