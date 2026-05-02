/**
 * @file PluginContext.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the singleton that owns GCC plugin state, analyzers, and the CodeModel.
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

#pragma once

#include <memory> // unique_ptr
#include <vector> // vector

#include <gcc-plugin.h>
#include <tree-pass.h> // register_pass_info

#include "AnalysisContext.hpp"
#include "AnalysisManager.hpp"
#include "CodeModel.hpp"
#include "DiagnosticReporter.hpp"
#include "GCCAdapter.hpp"
#include "GCCDiagnosticReporter.hpp"
#include "IAnalyzer.hpp"
#include "PluginArgs.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

// Meyers' Singleton
/**
 * Singleton that owns the live GCC plugin state for one compilation process.
 */
class PluginContext
{
  public:
    PluginContext(PluginContext &other) = delete;
    void operator=(const PluginContext &) = delete;

    static PluginContext &getInstance();

    bool initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version);

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
    AnnotationServices::AnalysisManager shared_analysis_manager;

    PluginContext() = default;

    void init_print(const plugin_gcc_version *version);
    void load_analyzer(const std::string &path, const std::string &analyzer_args, const std::string &plugin_full_name);

    static void on_plugin_finish(void *gcc_data, void *user_data);
};

} // namespace CodeListener::CompilerAbstractionLayer
