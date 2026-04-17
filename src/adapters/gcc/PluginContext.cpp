#include <dlfcn.h>

#include <cl_analyzer_api.h>
#include <cl_native_analyzer_api.h>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "AnalysisContext.hpp"
#include "DOTExporter.hpp"
#include "JSONExporter.hpp"
#include "LegacyPredatorBridge.hpp"
#include "NativeAnalyzerBridge.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"
#include "PPExporter.hpp"

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

    // load external analyzer (e.g. libsl_analyzer.so) if requested
    if (args->load_analyzer.has_value())
    {
        const std::string &an_args = args->analyzer_args.has_value() ? args->analyzer_args.value() : "";
        load_analyzer(args->load_analyzer.value(), an_args, plugin_info->full_name ? plugin_info->full_name : "");
    }

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

void PluginContext::load_analyzer(const std::string &path, const std::string &analyzer_args,
                                  const std::string &plugin_full_name)
{
    // dlopen the analyzer shared library
    void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
        reporter.report(Core::DiagnosticLevel::Error,
                        std::string("Could not load analyzer '") + path + "': " + dlerror());
        return;
    }
    analyzer_dl_handle = handle;

    // check for the native (CodeModel-level) API
    auto get_native = reinterpret_cast<const cl_native_analyzer_api_t *(*)()>(dlsym(handle, "cl_get_native_api"));
    if (get_native)
    {
        const cl_native_analyzer_api_t *napi = get_native();
        if (napi && napi->api_version == CL_NATIVE_API_VERSION)
        {
            analyzers.push_back(std::make_unique<NativeAnalyzerBridge>(napi, analyzer_args));
            reporter.report(Core::DiagnosticLevel::Info, "Native analyzer loaded from '" + path + "'");
            return;
        }
        else if (napi)
        {
            reporter.report(Core::DiagnosticLevel::Warning,
                            "Native analyzer API version mismatch in '" + path + "', ignoring native API");
        }
    }

    // fall back to the legacy cl_get_analyzer_api
    auto get_api = reinterpret_cast<const cl_analyzer_api_t *(*)()>(dlsym(handle, "cl_get_analyzer_api"));
    if (!get_api)
    {
        reporter.report(Core::DiagnosticLevel::Error,
                        std::string("Analyzer '") + path +
                            "' exports neither 'cl_get_native_api' nor 'cl_get_analyzer_api'");
        return;
    }

    const cl_analyzer_api_t *api = get_api();
    if (!api)
    {
        reporter.report(Core::DiagnosticLevel::Error, "cl_get_analyzer_api() returned NULL");
        return;
    }
    if (api->api_version != CL_ANALYZER_API_VERSION)
    {
        reporter.report(Core::DiagnosticLevel::Error, std::string("Analyzer API version mismatch: plugin expects ") +
                                                          std::to_string(CL_ANALYZER_API_VERSION) +
                                                          ", analyzer reports " + std::to_string(api->api_version) +
                                                          " in '" + path + "'");
        return;
    }

    struct cl_code_listener *listener = api->create(analyzer_args.empty() ? nullptr : analyzer_args.c_str(),
                                                    plugin_full_name.empty() ? nullptr : plugin_full_name.c_str());
    if (!listener)
    {
        reporter.report(Core::DiagnosticLevel::Error, "Analyzer create() returned NULL");
        return;
    }

    analyzers.push_back(std::make_unique<LegacyPredatorBridge>(listener));

    // legacy analyzer may also optionally export the native API
    if (get_native)
    {
        const cl_native_analyzer_api_t *napi = get_native();
        if (napi && napi->api_version == CL_NATIVE_API_VERSION)
        {
            analyzers.push_back(std::make_unique<NativeAnalyzerBridge>(napi, analyzer_args));
            reporter.report(Core::DiagnosticLevel::Info, "Native analyzer API loaded from '" + path + "'");
        }
        else if (napi)
        {
            reporter.report(Core::DiagnosticLevel::Warning,
                            "Native analyzer API version mismatch in '" + path + "', ignoring native API");
        }
    }

    reporter.report(Core::DiagnosticLevel::Info, "Analyzer loaded from '" + path + "'");
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
        CodeListener::Exporters::DOTExporter dot_exporter(args->gen_dot_file.value(), args->gen_dot_verbosity);
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

    // feed the model to every loaded analyzer via a shared AnalysisContext
    AnalysisContext ctx(reporter, PluginContext::getInstance().shared_analysis_manager);
    for (auto &analyzer : PluginContext::getInstance().analyzers)
    {
        analyzer->analyze(model, ctx);
    }

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin finished");
}

} // namespace CodeListener::CompilerAbstractionLayer
