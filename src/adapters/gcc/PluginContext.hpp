#pragma once

#include <memory> // unique_ptr
#include <vector> // vector

#include <gcc-plugin.h>
#include <tree-pass.h> // register_pass_info

#include "CodeModel.hpp"
#include "DiagnosticReporter.hpp"
#include "GCCAdapter.hpp"
#include "GCCDiagnosticReporter.hpp"
#include "IAnalyzer.hpp"
#include "PluginArgs.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

// Meyers' Singleton
class PluginContext
{
  public:
    PluginContext(PluginContext &other) = delete;
    void operator=(const PluginContext &) = delete;

    static PluginContext &getInstance();

    void initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version);

    const PluginArgs *getArgs() const;
    Core::DiagnosticReporter &getDiagnosticReporter();
    Core::CodeModel &getCodeModel();
    GCCAdapter *getAdapter();

  private:
    std::unique_ptr<PluginArgs> args;
    std::unique_ptr<GCCAdapter> adapter;
    GCCDiagnosticReporter reporter;
    Core::CodeModel model;
    static struct plugin_info plugin_info;

    void *analyzer_dl_handle{nullptr};
    std::vector<std::unique_ptr<Core::IAnalyzer>> analyzers;

    PluginContext() = default;

    void init_print(const plugin_gcc_version *version);
    void load_analyzer(const std::string &path, const std::string &analyzer_args, const std::string &plugin_full_name);

    static void on_plugin_finish(void *gcc_data, void *user_data);
};

} // namespace CodeListener::CompilerAbstractionLayer
