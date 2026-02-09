#include <gcc-plugin.h> // for plugin_init and plugin_is_GPL_compatible
#include <iostream>     // for std::cout

// required by GCC to indicate that the plugin is GPL compatible
int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
    std::cout << "hello from GCC plugin\n";

    return 0;
}
