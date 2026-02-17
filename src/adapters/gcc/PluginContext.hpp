#pragma once

#include <memory> // unique_ptr

#include <gcc-plugin.h>
#include <tree-pass.h> // register_pass_info

#include "CodeModel.hpp"
#include "DiagnosticReporter.hpp"
#include "GCCDiagnosticReporter.hpp"
#include "PluginArgs.hpp"

namespace CodeListener
{

namespace CompilerAbstractionLayer
{

// Meyers' Singleton
class PluginContext
{
  public:
    PluginContext(PluginContext &other) = delete;
    void operator=(const PluginContext &) = delete;

    static PluginContext &getInstance();

    void initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version);

    const PluginArgs *getArgs() const
    {
        return args.get();
    }

    Core::DiagnosticReporter &getDiagnosticReporter()
    {
        return reporter;
    }

    Core::CodeModel &getCodeModel()
    {
        return model;
    }

  private:
    std::unique_ptr<PluginArgs> args;
    GCCDiagnosticReporter reporter;
    Core::CodeModel model;
    static struct plugin_info plugin_info;

    PluginContext() = default;

    void init_print(const plugin_gcc_version *version);

    static void on_plugin_finish(void *gcc_data, void *user_data);
};

} // namespace CompilerAbstractionLayer

} // namespace CodeListener
