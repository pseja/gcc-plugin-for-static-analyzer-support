/**
 * @file register_plugin.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Defines the GCC plugin entry point and early option handling.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream> // std::cerr
#include <cstring>  // std::strcmp

#include <gcc-plugin.h>     // plugin_init, plugin_is_GPL_compatible
#include <plugin-version.h> // gcc_version

#include "PluginArgs.hpp"
#include "PluginContext.hpp"
#include "StderrDiagnosticReporter.hpp"
#include "register_plugin.hpp"

namespace
{

bool has_compatible_base_version(const plugin_gcc_version *version)
{
    return version && version->basever && gcc_version.basever &&
           std::strcmp(version->basever, gcc_version.basever) == 0;
}

const char *safe_basever(const plugin_gcc_version *version)
{
    return (version && version->basever) ? version->basever : "<unknown>";
}

} // namespace

// required by GCC to indicate that the plugin is GPL-compatible
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

    CodeListener::Core::StderrDiagnosticReporter early_reporter;
    CodeListener::CompilerAbstractionLayer::PluginArgs early_args(plugin_info, early_reporter);
    if (!early_args.valid)
    {
        return 1;
    }

    if (early_args.version)
    {
        std::cout << early_args.versionText() << "\n";
        return 0;
    }
    if (early_args.help)
    {
        std::cout << early_args.helpText() << "\n";
        return 0;
    }

    if (!has_compatible_base_version(version))
    {
        std::cerr << "Incompatible GCC version: This plugin was compiled against GCC " << safe_basever(&gcc_version)
                  << ", but the current GCC version is " << safe_basever(version) << "\n";

        return 1;
    }

    CodeListener::CompilerAbstractionLayer::PluginContext &context =
        CodeListener::CompilerAbstractionLayer::PluginContext::getInstance();
    if (!context.initialize(plugin_info, version))
    {
        return 1;
    }

    return 0;
}
