/**
 * @file PluginContext.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the singleton that owns GCC plugin state, analyzers, and the CodeModel.
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

#include <fstream>

#include <dlfcn.h>
#include <unistd.h>

#include <cl_analyzer_api.h>
#include <cl_native_analyzer_api.h>

#include <gcc-plugin.h>
#include <context.h>
#include <tree-pass.h>

#include "AnalysisContext.hpp"
#include "DOTExporter.hpp"
#include "GCCFrontend.hpp"
#include "JSONExporter.hpp"
#include "LegacyPredatorBridge.hpp"
#include "NativeAnalyzerBridge.hpp"
#include "Pass.hpp"
#include "PluginContext.hpp"
#include "PPExporter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

namespace
{

bool writePidFile(const std::string &pid_file, Core::DiagnosticReporter &reporter)
{
    std::ofstream stream(pid_file);
    if (!stream.is_open())
    {
        reporter.report(Core::DiagnosticLevel::Error, "Failed to open PID file '" + pid_file + "' for writing");
        return false;
    }

    stream << getpid() << '\n';
    if (!stream)
    {
        reporter.report(Core::DiagnosticLevel::Error, "Failed to write PID file '" + pid_file + "'");
        return false;
    }

    return true;
}

} // namespace

struct plugin_info PluginContext::plugin_info = {
    .version = "0.1",
    .help = "Translate GCC GIMPLE into the CodeListener CodeModel and run configured exporters or analyzers. Use "
            "-fplugin-arg-libcl_gcc-help for the full option list.",
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

bool PluginContext::initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version)
{
    args = std::make_unique<PluginArgs>(plugin_info, reporter);
    if (!args->valid)
    {
        return false;
    }
    if (args->verbose >= 2)
    {
        reporter.setVerbosityLevel(Core::DiagnosticLevel::Debug);
    }
    else if (args->verbose >= 1)
    {
        reporter.setVerbosityLevel(Core::DiagnosticLevel::Info);
    }

    if (args->pid_file.has_value() && !writePidFile(args->pid_file.value(), reporter))
    {
        return false;
    }

    adapter = std::make_unique<GCCAdapter>(model, reporter);

    init_print(version);

    // metadata
    register_callback(plugin_info->base_name, PLUGIN_INFO, nullptr, &PluginContext::plugin_info);

    Pass *p = new Pass(g, *adapter);
    // NOTE: for PHI nodes - change reference_pass_name to "ssa", predator doesn't work with PHI nodes, but the
    // implementation is there for future analyzers
    static struct register_pass_info cl_plugin_pass = {
        .pass = p, .reference_pass_name = "cfg", .ref_pass_instance_number = 0, .pos_op = PASS_POS_INSERT_AFTER};

    // register callbacks
    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr, &cl_plugin_pass);
    register_callback(plugin_info->base_name, PLUGIN_START_UNIT, on_start_unit, this);
    register_callback(plugin_info->base_name, PLUGIN_FINISH, on_plugin_finish, this);

    // load external analyzer (e.g. libsl_analyzer.so) if requested
    if (args->use_analyzer && args->load_analyzer.has_value())
    {
        const std::string &an_args = args->analyzer_args.has_value() ? args->analyzer_args.value() : "";
        load_analyzer(args->load_analyzer.value(), an_args, plugin_info->full_name ? plugin_info->full_name : "");
    }
    else if (!args->use_analyzer && args->load_analyzer.has_value())
    {
        reporter.report(Core::DiagnosticLevel::Info,
                        "Dry-run requested; skipping analyzer load from '" + args->load_analyzer.value() + "'");
    }

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin initialized");

    return true;
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

    PluginContext &plugin_context = PluginContext::getInstance();
    Core::DiagnosticReporter &reporter = plugin_context.getDiagnosticReporter();
    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin starting to export");

    const PluginArgs *args = plugin_context.getArgs();
    if (!args)
    {
        reporter.report(Core::DiagnosticLevel::Error,
                        "Code Listener GCC plugin finished, but weren't able to retrieve arguments");
        return;
    }

    const Core::CodeModel &model = plugin_context.getCodeModel();

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

    // feed the model to every loaded analyzer via GCCFrontend
    if (args->use_analyzer && !plugin_context.analyzers.empty())
    {
        GCCFrontend frontend(model);
        AnalysisContext ctx(reporter, plugin_context.shared_analysis_manager);
        if (!frontend.run(plugin_context.analyzers, ctx))
        {
            reporter.report(Core::DiagnosticLevel::Error, "One or more analyzers reported errors");
        }
    }
    else if (!args->use_analyzer)
    {
        reporter.report(Core::DiagnosticLevel::Info, "Dry-run requested; skipping analyzer execution");
    }

    reporter.report(Core::DiagnosticLevel::Info, "Code Listener GCC plugin finished");
}

} // namespace CodeListener::CompilerAbstractionLayer
