#include <string_view> // std::string_view
#include <charconv>    // std::from_chars

#include <gcc-plugin.h> // plugin_name_args

#include "PluginArgs.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

PluginArgs::PluginArgs(const plugin_name_args *plugin_info, Core::DiagnosticReporter &reporter)
    : base_name(plugin_info->base_name ? plugin_info->base_name : ""),
      full_name(plugin_info->full_name ? plugin_info->full_name : "")
{
    for (int i = 0; i < plugin_info->argc; i++)
    {
        std::string_view key = plugin_info->argv[i].key ? plugin_info->argv[i].key : "";
        std::string_view value = plugin_info->argv[i].value ? plugin_info->argv[i].value : "";

        raw_args.emplace_back(key, value);

        if (key == "verbose")
        {
            if (value.empty())
            {
                verbose = 1;
            }
            else
            {
                auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), verbose);
                if (ec != std::errc{})
                {
                    reporter.report(Core::DiagnosticLevel::Error,
                                    "CodeListener: error: invalid integer for 'verbose': " + std::string(value));
                    valid = false;
                }
            }
        }
        else if (key == "version")
        {
            version = true;
        }
        else if (key == "help")
        {
            help = true;
        }
        else if (key == "args")
        {
            analyzer_args = value;
        }
        else if (key == "dry-run")
        {
            use_analyzer = false;
        }
        else if (key == "dump-pp")
        {
            dump_pp_file = value;
        }
        else if (key == "dump-types")
        {
            dump_types = true;
        }
        else if (key == "gen-dot")
        {
            gen_dot_file = value;
        }
        else if (key == "gen-json")
        {
            gen_json_file = value;
        }
        else if (key == "preserve-ec")
        {
            preserve_ec = true;
        }
        else if (key == "pid-file")
        {
            if (value.empty())
            {
                reporter.report(Core::DiagnosticLevel::Error,
                                "CodeListener: error: mandatory value omitted for pid-file");
                valid = false;
            }
            else
            {
                pid_file = value;
            }
        }
        else if (key == "type-dot")
        {
            if (value.empty())
            {
                reporter.report(Core::DiagnosticLevel::Error,
                                "CodeListener: error: mandatory value omitted for type-dot");
                valid = false;
            }
            else
            {
                type_dot_file = value;
            }
        }
        else
        {
            reporter.report(Core::DiagnosticLevel::Error,
                            "CodeListener: error: unhandled plug-in argument: " + std::string(key));
            valid = false;
        }
    }
}

void PluginArgs::print(Core::DiagnosticReporter &reporter) const
{
    reporter.report(Core::DiagnosticLevel::Info, "PluginArgs:");
    reporter.report(Core::DiagnosticLevel::Info, "  Base name: '" + base_name + "'");
    reporter.report(Core::DiagnosticLevel::Info, "  Full name: '" + full_name + "'");
    reporter.report(Core::DiagnosticLevel::Info, "  Arguments:");
    for (const auto &[key, value] : raw_args)
    {
        reporter.report(Core::DiagnosticLevel::Info, "    Key: '" + key + "', Value: '" + value + "'");
    }
}

} // namespace CodeListener::CompilerAbstractionLayer
