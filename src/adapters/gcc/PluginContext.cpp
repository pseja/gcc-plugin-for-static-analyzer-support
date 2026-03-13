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
    adapter = std::make_unique<GCCAdapter>(model, reporter);

    init_print(version);

    // metadata
    register_callback(plugin_info->base_name, PLUGIN_INFO, nullptr, &PluginContext::plugin_info);

    Pass *p = new Pass(g, *adapter);
    static struct register_pass_info cl_plugin_pass = {
        .pass = p, .reference_pass_name = "cfg", .ref_pass_instance_number = 0, .pos_op = PASS_POS_INSERT_AFTER};
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

const PluginArgs *PluginContext::getArgs() const
{
    return args.get();
}

Core::DiagnosticReporter &PluginContext::getDiagnosticReporter()
{
    return reporter;
}

Core::CodeModel &PluginContext::getCodeModel()
{
    return model;
}

GCCAdapter *PluginContext::getAdapter()
{
    return adapter.get();
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

    std::cerr << "Code Listener GCC plugin starting to export\n";

    const PluginArgs *args = PluginContext::getInstance().getArgs();
    if (!args)
    {
        std::cerr << "Code Listener GCC plugin finished, but weren't able to retrieve arguments\n";
        return;
    }

    if (args->gen_json_file.has_value())
    {
        std::ofstream json_out(args->gen_json_file.value());
        if (json_out.is_open())
        {
            CodeListener::Exporters::JSONExporter exporter(json_out);
            exporter.exportModel(PluginContext::getInstance().getCodeModel());
            std::cerr << "Exported JSON to " << args->gen_json_file.value() << "\n";
        }
        else
        {
            std::cerr << "Failed to open JSON file: " << args->gen_json_file.value() << "\n";
        }
    }

    if (args->gen_dot_file.has_value())
    {
        std::ofstream dot_out(args->gen_dot_file.value());
        if (dot_out.is_open())
        {
            CodeListener::Exporters::DOTExporter dot_exporter(dot_out);
            dot_exporter.exportModel(PluginContext::getInstance().getCodeModel());
            std::cerr << "Exported DOT to " << args->gen_dot_file.value() << "\n";
        }
        else
        {
            std::cerr << "Failed to open DOT file: " << args->gen_dot_file.value() << "\n";
        }
    }

    std::cerr << "Code Listener GCC plugin finished\n";
}

} // namespace CodeListener::CompilerAbstractionLayer
