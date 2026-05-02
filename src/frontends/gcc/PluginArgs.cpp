#include <string_view> // std::string_view
#include <charconv>    // std::from_chars
#include <sstream>     // std::ostringstream

#include <gcc-plugin.h> // plugin_name_args

#include "PluginArgs.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

namespace
{

constexpr std::string_view plugin_version = "0.1";

std::string pluginBaseName(const PluginArgs &args)
{
    return args.base_name.empty() ? "libcl_gcc" : args.base_name;
}

std::string pluginPath(const PluginArgs &args)
{
    return args.full_name.empty() ? pluginBaseName(args) : args.full_name;
}

} // namespace

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
                                    "Invalid integer for 'verbose': " + std::string(value));
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
        else if (key == "load-analyzer")
        {
            if (value.empty())
            {
                reporter.report(Core::DiagnosticLevel::Error, "Mandatory value omitted for load-analyzer argument");
                valid = false;
            }
            else
            {
                load_analyzer = value;
            }
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
        else if (key == "gen-dot-verbosity")
        {
            if (value == "CLEAN")
            {
                gen_dot_verbosity = Exporters::DotVerbosity::CLEAN;
            }
            else if (value == "COMPACT")
            {
                gen_dot_verbosity = Exporters::DotVerbosity::COMPACT;
            }
            else if (value == "FULL" || value.empty())
            {
                gen_dot_verbosity = Exporters::DotVerbosity::FULL;
            }
            else
            {
                reporter.report(Core::DiagnosticLevel::Warning, std::string("Unknown gen-dot-verbosity value '") +
                                                                    std::string(value) + "'; using FULL");
            }
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
                reporter.report(Core::DiagnosticLevel::Error, "Mandatory value omitted for pid-file argument");
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
                reporter.report(Core::DiagnosticLevel::Error, "Mandatory value omitted for type-dot argument");
                valid = false;
            }
            else
            {
                type_dot_file = value;
            }
        }
        else
        {
            reporter.report(Core::DiagnosticLevel::Error, "Unhandled plug-in argument: " + std::string(key));
            valid = false;
        }
    }
}

std::string PluginArgs::versionText() const
{
    return pluginBaseName(*this) + " " + std::string(plugin_version);
}

std::string PluginArgs::helpText() const
{
    const std::string name = pluginBaseName(*this);
    std::ostringstream out;

    out << versionText() << "\n\n";
    out << "Usage: gcc -fplugin=" << pluginPath(*this) << " [OPTIONS] ...\n\n";
    out << "Implemented options:\n";
    out << "    -fplugin-arg-" << name << "-help\n";
    out << "    -fplugin-arg-" << name << "-version\n";
    out << "    -fplugin-arg-" << name << "-load-analyzer=PATH      - load a native or legacy analyzer library\n";
    out << "    -fplugin-arg-" << name << "-args=ANALYZER_ARGS        - forward arguments to the loaded analyzer\n";
    out << "    -fplugin-arg-" << name << "-dry-run                 - skip analyzer loading and execution\n";
    out << "    -fplugin-arg-" << name << "-dump-pp[=OUTPUT_FILE]   - export the pretty-printed CodeModel\n";
    out << "    -fplugin-arg-" << name << "-gen-json[=OUTPUT_FILE]  - export the CodeModel as JSON\n";
    out << "    -fplugin-arg-" << name << "-gen-dot[=OUTPUT_FILE]   - export the CodeModel as DOT\n";
    out << "    -fplugin-arg-" << name << "-gen-dot-verbosity=LEVEL   - set DOT verbosity to CLEAN, COMPACT, or FULL\n";
    out << "    -fplugin-arg-" << name << "-pid-file=FILE           - write the plugin process PID to FILE\n";
    out << "    -fplugin-arg-" << name << "-verbose[=LEVEL]         - turn on informational or debug diagnostics\n\n";
    out << "Compatibility flags recognized but not yet implemented:\n";
    out << "    -fplugin-arg-" << name << "-dump-types\n";
    out << "    -fplugin-arg-" << name << "-preserve-ec\n";
    out << "    -fplugin-arg-" << name << "-type-dot=FILE\n";

    return out.str();
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
