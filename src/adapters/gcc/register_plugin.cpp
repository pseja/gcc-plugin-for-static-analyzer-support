#include <iostream> // std::cerr

#include <gcc-plugin.h>     // plugin_init, plugin_is_GPL_compatible
#include <plugin-version.h> // gcc_version

#include "PluginContext.hpp"
#include "register_plugin.hpp"

// required by GCC to indicate that the plugin is GPL compatible
int plugin_is_GPL_compatible;

namespace CodeListener::CompilerAbstractionLayer
{

void print_info(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    std::cerr << "--- plugin info ---\n";
    std::cerr << "plugin name: " << plugin_info->base_name << "\n";
    std::cerr << "full plugin name: " << plugin_info->full_name << "\n";
    std::cerr << "argument count: " << plugin_info->argc << "\n";
    for (int i = 0; i < plugin_info->argc; ++i)
    {
        std::cerr << "arg " << i << ": key=" << plugin_info->argv[i].key << ", value=" << plugin_info->argv[i].value
                  << "\n";
    }
    // std::cerr << "plugin version: " << plugin_info->version << "\n";
    // std::cerr << "plugin help: " << plugin_info->help << "\n";

    std::cerr << "\n--- GCC version info ---\n";
    std::cerr << "basever: " << version->basever << "\n";
    std::cerr << "datestamp: " << version->datestamp << "\n";
    std::cerr << "devphase: " << version->devphase << "\n";
    std::cerr << "revision: " << version->revision << "\n";
    std::cerr << "configuration arguments: " << version->configuration_arguments << "\n";
}

} // namespace CodeListener::CompilerAbstractionLayer

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    // CodeListener::CompilerAbstractionLayer::print_info(plugin_info, version);

    // FIXME: old cl had less strict version check for predator
    if (!plugin_default_version_check(version, &gcc_version))
    {
        std::cerr << "Incompatible GCC version: This plugin was compiled for version " << GCCPLUGIN_VERSION_MAJOR << "."
                  << GCCPLUGIN_VERSION_MINOR << ", but the current GCC version is " << version->basever << "\n";

        return 1;
    }

    CodeListener::CompilerAbstractionLayer::PluginContext &context =
        CodeListener::CompilerAbstractionLayer::PluginContext::getInstance();
    context.initialize(plugin_info, version);

    return 0;
}
