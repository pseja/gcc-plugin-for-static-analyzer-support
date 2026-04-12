#pragma once

#include <string>   // std::string
#include <vector>   // std::vector, std::pair
#include <optional> // std::optional

#include <gcc-plugin.h> // plugin_name_args

#include "DiagnosticReporter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

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
    std::optional<std::string> pid_file;
    std::optional<std::string> type_dot_file;

    explicit PluginArgs(const plugin_name_args *plugin_info, Core::DiagnosticReporter &reporter);

    void print(Core::DiagnosticReporter &reporter) const;
};

} // namespace CodeListener::CompilerAbstractionLayer
