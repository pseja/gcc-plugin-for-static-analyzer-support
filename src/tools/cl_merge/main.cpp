/**
 * @file main.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Merge multiple per-TU CodeModel JSON files into a single whole-program CodeModel.
 * @date 2026-04-16
 *
 * Usage:
 *   cl_merge <input1.json> [input2.json ...] -o <output.json>
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
#include <iostream>
#include <string>
#include <vector>

#include "CallGraph.hpp"
#include "JSONExporter.hpp"
#include "JSONImporter.hpp"
#include "ModelMerger.hpp"

static void printUsage(const char *program_name)
{
    std::cerr << "Usage: " << program_name << " <input1.json> [input2.json ...] -o <output.json>\n"
              << "\n"
              << "Merge per-translation-unit CodeModel JSON files into a single\n"
              << "whole-program CodeModel and write it to <output.json>.\n";
}

int main(int argc, char **argv)
{
    // parse cmd arguments
    if (argc < 4)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    std::vector<std::string> inputs;
    std::string output;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: -o requires an argument\n";
                printUsage(argv[0]);
                return EXIT_FAILURE;
            }
            output = argv[++i];
        }
        else
        {
            inputs.push_back(std::move(arg));
        }
    }

    if (inputs.empty())
    {
        std::cerr << "Error: no input files specified\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    if (output.empty())
    {
        std::cerr << "Error: no output file specified (-o <file>)\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // import all per-TU models
    std::vector<CodeListener::Core::CodeModel> models;
    models.reserve(inputs.size());

    for (const auto &path : inputs)
    {
        try
        {
            std::cerr << "Importing: " << path << "\n";
            models.push_back(CodeListener::Exporters::JSONImporter::importFromFile(path));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error reading '" << path << "': " << e.what() << "\n";
            return EXIT_FAILURE;
        }
    }

    // merge
    std::cerr << "Merging " << models.size() << " translation unit(s)...\n";
    CodeListener::Core::CodeModel merged = CodeListener::Exporters::ModelMerger::merge(std::move(models));

    // export
    try
    {
        std::cerr << "Writing: " << output << "\n";
        CodeListener::Exporters::JSONExporter exporter(output);
        exporter.exportModel(merged);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error writing '" << output << "': " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    std::cerr << "Successfully exported: Types=" << merged.getTypes().size()
              << " Variables=" << merged.getVariables().size() << " Functions=" << merged.getFunctions().size()
              << " Blocks=" << merged.getBlocks().size() << " Instructions=" << merged.getInstructions().size() << "\n";

    return EXIT_SUCCESS;
}
