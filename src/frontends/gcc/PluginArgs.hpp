/**
 * @file PluginArgs.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares parsing and rendering of GCC plugin command-line arguments.
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

#include <string>   // std::string
#include <vector>   // std::vector, std::pair
#include <optional> // std::optional

#include <gcc-plugin.h> // plugin_name_args

#include "DiagnosticReporter.hpp"
#include "DOTVerbosity.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Owns the parsed GCC plugin command-line arguments and derived option state.
 */
struct PluginArgs
{
    /** Basename under which GCC registered the plugin. */
    std::string base_name;

    /** Full path of the loaded plugin shared library. */
    std::string full_name;

    /** Raw key/value arguments received from GCC before normalization. */
    std::vector<std::pair<std::string, std::string>> raw_args;

    /** Requested diagnostic verbosity level. */
    int verbose{0};

    /** Whether the user requested version output. */
    bool version{false};

    /** Whether the user requested help output. */
    bool help{false};

    /** Whether analyzer loading and execution should run normally. */
    bool use_analyzer{true};

    /** Compatibility flag matching the historical `preserve-ec` option. */
    bool preserve_ec{false};

    /** Compatibility flag matching the historical `dump-types` option. */
    bool dump_types{false};

    /** Whether parsing completed without fatal option errors. */
    bool valid{true};

    /** Optional opaque argument string forwarded to the analyzer. */
    std::optional<std::string> analyzer_args;

    /** Optional path to a shared library implementing an analyzer. */
    std::optional<std::string> load_analyzer;

    /** Optional output path for the pretty-printed exporter. */
    std::optional<std::string> dump_pp_file;

    /** Optional output path for JSON export. */
    std::optional<std::string> gen_json_file;

    /** Optional output path for DOT export. */
    std::optional<std::string> gen_dot_file;

    /** Requested verbosity level for DOT export. */
    Exporters::DotVerbosity gen_dot_verbosity{Exporters::DotVerbosity::CLEAN};

    /** Optional path where the plugin should write its PID. */
    std::optional<std::string> pid_file;

    /** Optional path for compatibility-oriented type graph export. */
    std::optional<std::string> type_dot_file;

    /**
     * Parse and normalize the GCC plugin argument vector.
     *
     * @param plugin_info Raw plugin information provided by GCC.
     * @param reporter Diagnostic sink used for invalid option reporting.
     */
    explicit PluginArgs(const plugin_name_args *plugin_info, Core::DiagnosticReporter &reporter);

    /**
     * Render the short plugin version banner.
     *
     * @return Human-readable version string.
     */
    [[nodiscard]] std::string versionText() const;

    /**
     * Render the plugin help text.
     *
     * @return Human-readable help text describing supported options.
     */
    [[nodiscard]] std::string helpText() const;

    /**
     * Dump the parsed argument state to the diagnostic reporter.
     *
     * @param reporter Diagnostic sink receiving the formatted state dump.
     */
    void print(Core::DiagnosticReporter &reporter) const;
};

} // namespace CodeListener::CompilerAbstractionLayer
