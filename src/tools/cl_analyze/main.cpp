/**
 * @file main.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Run a native CodeListener analyzer on a multi TU JSON CodeModel.
 *
 * Usage:
 *   cl_analyze <model.json> --analyzer=<path/to/libanalyzer.so> [--args=<string>]
 *
 * Example (generate a whole-program call graph):
 *   cl_merge a.json b.json c.json -o merged.json
 *   cl_analyze merged.json --analyzer=libcl_callgraph_dot.so --args=whole-program-cg.dot
 *
 * @date 2026-04-16
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

#include <cstdlib>
#include <dlfcn.h>
#include <iostream>
#include <string>

#include <cl_native_analyzer_api.h>

#include "AnalysisContext.hpp"
#include "AnalysisManager.hpp"
#include "CodeModel.hpp"
#include "DOTExporter.hpp"
#include "JSONImporter.hpp"
#include "PPExporter.hpp"
#include "StderrDiagnosticReporter.hpp"

static void printUsage(const char *program_name)
{
    std::cerr << "Usage: " << program_name << " <model.json> [options] [--analyzer=<lib.so> [--args=<string>]]\n"
              << "\n"
              << "  model.json              - Path to a CodeModel JSON file (single TU or merged).\n"
              << "  --analyzer=<lib>        - Path to a shared library implementing cl_get_native_api.\n"
              << "  --args=<string>         - Argument string forwarded to the analyzer (optional).\n"
              << "  --gen-dot=<file>        - Export the model as a DOT CFG graph.\n"
              << "  --gen-dot-verbosity=X   - DOT verbosity: CLEAN, COMPACT, or FULL (default: FULL).\n"
              << "  --gen-pp=<file>         - Export the model as a pretty-printed text listing.\n";
}

int main(int argc, char *argv[])
{
    // parse cmd arguments
    if (argc < 3)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::string model_path;
    std::string analyzer_path;
    std::string analyzer_args;
    std::string gen_dot_file;
    std::string gen_pp_file;
    CodeListener::Exporters::DotVerbosity dot_verbosity = CodeListener::Exporters::DotVerbosity::FULL;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.rfind("--analyzer=", 0) == 0)
        {
            analyzer_path = arg.substr(11);
        }
        else if (arg.rfind("--args=", 0) == 0)
        {
            analyzer_args = arg.substr(7);
        }
        else if (arg.rfind("--gen-dot=", 0) == 0)
        {
            gen_dot_file = arg.substr(10);
        }
        else if (arg.rfind("--gen-dot-verbosity=", 0) == 0)
        {
            std::string v = arg.substr(20);
            if (v == "CLEAN")
            {
                dot_verbosity = CodeListener::Exporters::DotVerbosity::CLEAN;
            }
            else if (v == "COMPACT")
            {
                dot_verbosity = CodeListener::Exporters::DotVerbosity::COMPACT;
            }
            else if (v == "FULL")
            {
                dot_verbosity = CodeListener::Exporters::DotVerbosity::FULL;
            }
            else
            {
                std::cerr << "Warning: unknown --gen-dot-verbosity value '" << v << "'; using FULL\n";
            }
        }
        else if (arg.rfind("--gen-pp=", 0) == 0)
        {
            gen_pp_file = arg.substr(9);
        }
        else if (model_path.empty() && arg[0] != '-')
        {
            model_path = std::move(arg);
        }
        else
        {
            std::cerr << "Error: unrecognised argument: " << arg << "\n";
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (model_path.empty())
    {
        std::cerr << "Error: no model JSON file specified\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    if (analyzer_path.empty() && gen_dot_file.empty() && gen_pp_file.empty())
    {
        std::cerr << "Error: nothing to do, specify --analyzer, --gen-dot, or --gen-pp\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // load analyzer shared library (only when --analyzer was given)
    void *handle = nullptr;
    const cl_native_analyzer_api_t *api = nullptr;

    if (!analyzer_path.empty())
    {
        handle = dlopen(analyzer_path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle)
        {
            std::cerr << "Error: cannot load analyzer '" << analyzer_path << "': " << dlerror() << "\n";
            return EXIT_FAILURE;
        }

        void *sym = dlsym(handle, "cl_get_native_api");
        if (!sym)
        {
            std::cerr << "Error: '" << analyzer_path << "' does not export cl_get_native_api (not a native analyzer?)\n"
                      << "  dlerror: " << dlerror() << "\n";
            dlclose(handle);
            return EXIT_FAILURE;
        }

        const cl_native_analyzer_api_t *(*get_api)();
        static_assert(sizeof(sym) == sizeof(get_api), "function pointer size mismatch");
        __builtin_memcpy(&get_api, &sym, sizeof(get_api));

        api = get_api();
        if (!api)
        {
            std::cerr << "Error: cl_get_native_api() returned NULL\n";
            dlclose(handle);
            return EXIT_FAILURE;
        }
        if (api->api_version != CL_NATIVE_API_VERSION)
        {
            std::cerr << "Error: analyzer API version mismatch (expected " << CL_NATIVE_API_VERSION << ", got "
                      << api->api_version << ")\n";
            dlclose(handle);
            return EXIT_FAILURE;
        }
        if (!api->analyze)
        {
            std::cerr << "Error: analyzer->analyze is NULL\n";
            dlclose(handle);
            return EXIT_FAILURE;
        }
    }

    // import the CodeModel
    CodeListener::Core::CodeModel model;
    try
    {
        model = CodeListener::Exporters::JSONImporter::importFromFile(model_path);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error importing '" << model_path << "': " << e.what() << "\n";
        if (!analyzer_path.empty())
        {
            dlclose(handle);
        }
        return EXIT_FAILURE;
    }

    // built-in exports (do not require --analyzer)
    if (!gen_dot_file.empty())
    {
        CodeListener::Exporters::DOTExporter dot_exporter(gen_dot_file, dot_verbosity);
        dot_exporter.exportModel(model);
    }
    if (!gen_pp_file.empty())
    {
        CodeListener::Exporters::PPExporter pp_exporter(gen_pp_file);
        pp_exporter.exportModel(model);
    }

    // run the native analyzer (optional)
    if (!analyzer_path.empty())
    {
        CodeListener::Core::StderrDiagnosticReporter stderr_reporter;
        CodeListener::AnnotationServices::AnalysisManager am;
        CodeListener::AnalysisContext ctx(stderr_reporter, am);
        const char *args_cstr = analyzer_args.empty() ? nullptr : analyzer_args.c_str();
        bool ok = api->analyze(model, ctx, args_cstr);
        dlclose(handle);
        if (!ok || stderr_reporter.hadError())
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
