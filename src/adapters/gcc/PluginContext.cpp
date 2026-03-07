#include <cstdlib>
#include <fstream>
#include <iostream>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"

namespace CodeListener::CompilerAbstractionLayer
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

    std::cerr << "Code Listener GCC plugin initialized\n";
}

void PluginContext::init_print(const plugin_gcc_version *version)
{
    std::cerr << "Initializing Code Listener GCC plugin\n";
    std::cerr << "GCC version: " << version->basever << "\n";

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

    // TODO: export the model based on arguments
    if (const char *dot_file = std::getenv("CL_JSON_FILE"))
    {
        std::ofstream json_out(dot_file);
        if (json_out.is_open())
        {
            CodeListener::Exporters::JSONExporter exporter(json_out);
            exporter.exportModel(PluginContext::getInstance().getCodeModel());
            std::cerr << "Exported JSON to " << dot_file << "\n";
        }
        else
        {
            std::cerr << "Failed to open JSON file: " << dot_file << "\n";
        }
    }

    if (const char *dot_file = std::getenv("CL_DOT_FILE"))
    {
        std::ofstream dot_out(dot_file);
        if (dot_out.is_open())
        {
            CodeListener::Exporters::DOTExporter dot_exporter(dot_out);
            dot_exporter.exportModel(PluginContext::getInstance().getCodeModel());
            std::cerr << "Exported DOT to " << dot_file << "\n";
        }
        else
        {
            std::cerr << "Failed to open DOT file: " << dot_file << "\n";
        }
    }

    std::cerr << "Code Listener GCC plugin finished\n";
}

} // namespace CodeListener::CompilerAbstractionLayer
