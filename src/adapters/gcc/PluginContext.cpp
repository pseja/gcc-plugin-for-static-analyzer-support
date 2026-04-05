#include <cstdlib>
#include <fstream>
#include <sstream>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "AnalysisManager.hpp"
#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"
#include "../predator/PredatorAdapter.hpp"

extern "C"
{
    void cl_global_init_defaults(const char *app_name, int debug_level);
    struct cl_code_listener *cl_code_listener_create(const char *config_string);
    void cl_global_cleanup(void);
}

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
    AnnotationServices::AnalysisManager analysis_manager;

    // JSON export
    if (args->gen_json_file.has_value())
    {
        CodeListener::Exporters::JSONExporter exporter(args->gen_json_file.value(), &analysis_manager);
        exporter.exportModel(model);
        reporter.report(Core::DiagnosticLevel::Info, "Exported JSON to " + args->gen_json_file.value());
    }

    // DOT export
    if (args->gen_dot_file.has_value())
    {
        CodeListener::Exporters::DOTExporter dot_exporter(args->gen_dot_file.value(), &analysis_manager);
        dot_exporter.exportModel(model);
        reporter.report(Core::DiagnosticLevel::Info, "Exported DOT to " + args->gen_dot_file.value());
    }

    // skip cl (predator) pipeline when dry-run and no pp output requested
    if (!args->use_analyzer && !args->dump_pp_file.has_value())
    {
        reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin finished (dry-run)");
        return;
    }

    reporter.report(Core::DiagnosticLevel::Info, "Starting Predator pipeline...");

    cl_global_init_defaults("cl_gcc_adapter", 0);

    const char *listener = args->dump_types ? "pp_with_types" : "pp";
    const std::string out_file = args->dump_pp_file.value_or("");
    // full switch-unfolding when running the analyzer, minimal otherwise
    const char *clf = args->use_analyzer ? "unfold_switch,unify_labels_gl" : "unify_labels_fnc";

    std::ostringstream oss;
    oss << "listener=\"" << listener << "\" listener_args=\"" << out_file << "\" clf=\"" << clf << "\"";
    std::string config = oss.str();

    struct cl_code_listener *predator_listener = cl_code_listener_create(config.c_str());
    if (predator_listener)
    {
        CodeListener::Adapters::PredatorAdapter adapter(model, predator_listener);
        adapter.emit();
        reporter.report(Core::DiagnosticLevel::Info, "Predator pipeline finished.");
    }
    else
    {
        reporter.report(Core::DiagnosticLevel::Error, "Failed to create Predator listener");
    }
    cl_global_cleanup();

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin finished");
}

} // namespace CodeListener::CompilerAbstractionLayer
