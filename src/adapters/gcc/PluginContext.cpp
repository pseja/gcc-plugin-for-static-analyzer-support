#include <iostream>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "JSONExporter.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"

namespace CodeListener
{

namespace CompilerAbstractionLayer
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

    Pass p(g);
    static struct register_pass_info cl_plugin_pass = {
        .pass = &p, .reference_pass_name = "cfg", .ref_pass_instance_number = 0, .pos_op = PASS_POS_INSERT_AFTER};
    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr, &cl_plugin_pass);

    // TODO: register callbacks
    // register_callback(plugin_info->base_name, PLUGIN_START_UNIT, on_start_unit, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_START_PARSE_FUNCTION, on_start_function, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_TYPE, on_finish_type, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_PARSE_FUNCTION, on_finish_function, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_UNIT, on_finish_unit, nullptr);

    // cleanup
    register_callback(plugin_info->base_name, PLUGIN_FINISH, on_plugin_finish, this);

    std::cout << "Code Listener GCC plugin initialized\n";
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

    // TODO: run the analyzer/s here
    CodeListener::Exporters::JSONExporter exporter(std::cout);
    exporter.exportModel(PluginContext::getInstance().getCodeModel());

    std::cout << "Code Listener GCC plugin finished\n";
}

} // namespace CompilerAbstractionLayer

} // namespace CodeListener
