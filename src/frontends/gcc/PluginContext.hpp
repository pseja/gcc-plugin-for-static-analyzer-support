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

/**
 * Singleton that owns the live GCC plugin state for one compilation process.
 */
class PluginContext
{
  public:
    /** Prevent copying of the singleton state holder. */
    PluginContext(PluginContext &other) = delete;

    /** Prevent assigning the singleton state holder. */
    void operator=(const PluginContext &) = delete;

    /**
     * Access the process-wide plugin context instance.
     *
     * @return Singleton plugin context.
     */
    static PluginContext &getInstance();

    /**
     * Initialize the plugin context from GCC callback data.
     *
     * @param plugin_info GCC-provided plugin metadata and arguments.
     * @param version GCC version structure for the running compiler.
     *
     * @return `true` on success, `false` when initialization should abort the plugin load.
     */
    bool initialize(const plugin_name_args *plugin_info, const plugin_gcc_version *version);

    /** @return Parsed plugin arguments, or `nullptr` before initialization. */
    const PluginArgs *getArgs() const;

    /** @return Diagnostic reporter bound to GCC. */
    Core::DiagnosticReporter &getDiagnosticReporter();

    /** @return Mutable CodeModel populated by the GCC adapter. */
    Core::CodeModel &getCodeModel();

    /** @return Active GCC adapter instance, or `nullptr` before initialization. */
    GCCAdapter *getAdapter();

  private:
    /** Parsed command-line arguments controlling plugin behavior. */
    std::unique_ptr<PluginArgs> args;

    /** Adapter translating GCC IR into the CodeModel. */
    std::unique_ptr<GCCAdapter> adapter;

    /** Reporter that forwards diagnostics through GCC facilities. */
    GCCDiagnosticReporter reporter;

    /** Model populated during the current compilation unit. */
    Core::CodeModel model;

    /** Static metadata registered with GCC for the plugin. */
    static struct plugin_info plugin_info;

    /** Handle returned by `dlopen` for the currently loaded analyzer library. */
    void *analyzer_dl_handle{nullptr};

    /** Analyzer backends scheduled to run when the compilation unit finishes. */
    std::vector<std::unique_ptr<Core::IAnalyzer>> analyzers;

    /** Shared annotation cache reused by all analyzers in the current run. */
    AnnotationServices::AnalysisManager shared_analysis_manager;

    /** Default constructor hidden behind the singleton accessor. */
    PluginContext() = default;

    /**
     * Emit initialization diagnostics after the reporter and arguments are ready.
     *
     * @param version GCC version structure for the running compiler.
     */
    void initPrint(const plugin_gcc_version *version);

    /**
     * Load one analyzer shared library and wrap its exposed ABI in an `IAnalyzer` implementation.
     *
     * @param path Shared library path.
     * @param analyzer_args Opaque argument string forwarded to the analyzer.
     * @param plugin_full_name Full plugin path used by legacy analyzers.
     */
    void loadAnalyzer(const std::string &path, const std::string &analyzer_args, const std::string &plugin_full_name);

    /**
     * Final GCC callback that runs exports and analyzers once the unit has finished.
     *
     * @param gcc_data Unused GCC callback payload.
     * @param user_data User payload supplied during callback registration.
     */
    static void onPluginFinish(void *gcc_data, void *user_data);
};

} // namespace CodeListener::CompilerAbstractionLayer
