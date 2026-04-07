#include <fstream>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "PPExporter.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

// TODO: plugin arguments?
struct plugin_info PluginContext::plugin_info = {
    .version = "0.1",
    .help = "A plugin to support static analyzers like Predator",
};

static void on_start_unit(void *gcc_data, void *user_data)
{
    (void)gcc_data;

    PluginContext *ctx = static_cast<PluginContext *>(user_data);

    ctx->getCodeModel().setFilename(LOCATION_FILE(input_location));
}

PluginContext &PluginContext::getInstance()
{
    static PluginContext instance;
    return instance;
}

void PluginContext::initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version)
{
    args = std::make_unique<PluginArgs>(plugin_info, reporter);
    adapter = std::make_unique<GCCAdapter>(model, reporter);

    init_print(version);

    // metadata
    register_callback(plugin_info->base_name, PLUGIN_INFO, nullptr, &PluginContext::plugin_info);

    Pass *p = new Pass(g, *adapter);
    // NOTE: for PHI nodes - change reference_pass_name to "ssa", predator doesn't work with PHI nodes, but the
    // implementation is there for future analyzers
    static struct register_pass_info cl_plugin_pass = {
        .pass = p, .reference_pass_name = "cfg", .ref_pass_instance_number = 0, .pos_op = PASS_POS_INSERT_AFTER};
    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr, &cl_plugin_pass);

    // TODO: register callbacks
    register_callback(plugin_info->base_name, PLUGIN_START_UNIT, on_start_unit, this);

    // register_callback(plugin_info->base_name, PLUGIN_START_PARSE_FUNCTION, on_start_function, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_TYPE, on_finish_type, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_PARSE_FUNCTION, on_finish_function, nullptr);

    // register_callback(plugin_info->base_name, PLUGIN_FINISH_UNIT, on_finish_unit, nullptr);

    // cleanup
    register_callback(plugin_info->base_name, PLUGIN_FINISH, on_plugin_finish, this);

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin initialized");
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
    reporter.report(Core::DiagnosticLevel::Info, "Initializing Code Listener GCC plugin");
    reporter.report(Core::DiagnosticLevel::Info, std::string("GCC version: ") + version->basever);

    if (args)
    {
        args->print(reporter);
    }
}

void PluginContext::on_plugin_finish(void *gcc_data, void *user_data)
{
    (void)gcc_data;
    (void)user_data;

    Core::DiagnosticReporter &reporter = PluginContext::getInstance().getDiagnosticReporter();
    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin starting to export");

    const PluginArgs *args = PluginContext::getInstance().getArgs();
    if (!args)
    {
        reporter.report(Core::DiagnosticLevel::Error,
                        "Code Listener GCC plugin finished, but weren't able to retrieve arguments");
        return;
    }

    const Core::CodeModel &model = PluginContext::getInstance().getCodeModel();

    // JSON export
    if (args->gen_json_file.has_value())
    {
        CodeListener::Exporters::JSONExporter exporter(args->gen_json_file.value());
        exporter.exportModel(model);
        reporter.report(Core::DiagnosticLevel::Info, "Exported JSON to " + args->gen_json_file.value());
    }

    // DOT export
    if (args->gen_dot_file.has_value())
    {
        CodeListener::Exporters::DOTExporter dot_exporter(args->gen_dot_file.value());
        dot_exporter.exportModel(model);
        reporter.report(Core::DiagnosticLevel::Info, "Exported DOT to " + args->gen_dot_file.value());
    }

    // PP export
    if (args->dump_pp_file.has_value())
    {
        CodeListener::Exporters::PPExporter pp_exporter(args->dump_pp_file.value());
        pp_exporter.exportModel(model);
        reporter.report(Core::DiagnosticLevel::Info, "Exported PP to " + args->dump_pp_file.value());
    }

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin finished");
}

} // namespace CodeListener::CompilerAbstractionLayer
