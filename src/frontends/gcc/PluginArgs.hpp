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
    std::string base_name;
    std::string full_name;
    std::vector<std::pair<std::string, std::string>> raw_args;

    int verbose{0};
    bool version{false};
    bool help{false};
    bool use_analyzer{true};
    bool preserve_ec{false};
    bool dump_types{false};
    bool valid{true};

    std::optional<std::string> analyzer_args;
    std::optional<std::string> load_analyzer;
    std::optional<std::string> dump_pp_file;
    std::optional<std::string> gen_json_file;
    std::optional<std::string> gen_dot_file;
    Exporters::DotVerbosity gen_dot_verbosity{Exporters::DotVerbosity::CLEAN};
    std::optional<std::string> pid_file;
    std::optional<std::string> type_dot_file;

    explicit PluginArgs(const plugin_name_args *plugin_info, Core::DiagnosticReporter &reporter);

    [[nodiscard]] std::string versionText() const;
    [[nodiscard]] std::string helpText() const;
    void print(Core::DiagnosticReporter &reporter) const;
};

} // namespace CodeListener::CompilerAbstractionLayer
