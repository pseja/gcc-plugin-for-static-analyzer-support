/**
 * @file recursion_check.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Native analyzer that detects direct and mutual recursion using the CallGraph annotation.
 *
 * Usage (via cl_analyze):
 *   cl_analyze model.json --analyzer=libcl_recursion_check.so [--args=report.txt]
 *
 * or:
 *   make recursion FILE=&lt;src&gt; [ARGS=&lt;output.txt&gt;]
 *
 * @date 2026-04-18
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

#include <fstream>
#include <string>

#include <cl_native_analyzer_api.h>

#include "AnalysisContext.hpp"
#include "CallGraph.hpp"
#include "CodeModel.hpp"
#include "DiagnosticLevel.hpp"
#include "Function.hpp"

;

/**
 * Run the recursion_check analysis: detect direct and mutual recursion.
 *
 * @param model  Fully populated CodeModel for the translation unit.
 * @param ctx    Services available to the analyzer (reporter, annotation cache).
 * @param args   Output file path override; defaults to "recursion_report.txt" when NULL or empty.
 *
 * @return true on success; false if the output file could not be opened.
 */
static bool recursion_analyze(const CodeListener::Core::CodeModel &model, CodeListener::AnalysisContext &ctx,
                              const char *args)
{
    const std::string outpath = (args && args[0] != '\0') ? args : "recursion_report.txt";

    // request the call-graph annotation from the shared annotation cache
    const auto &cg = ctx.analysis_manager.getAnnotation<CodeListener::AnnotationServices::CallGraph>(model);

    std::ofstream out(outpath);
    if (!out.is_open())
    {
        ctx.reporter.report(CodeListener::Core::DiagnosticLevel::Error,
                            "recursion_check: cannot open output file '" + outpath + "'");
        return false;
    }

    bool found_any = false;

    for (const auto &scc : cg.topological_order)
    {
        // mutually recursive function group
        if (scc.size() > 1)
        {
            // build a comma-separated list of function names
            std::string names;
            for (std::size_t i = 0; i < scc.size(); ++i)
            {
                if (i > 0)
                {
                    names += ", ";
                }
                const CodeListener::Core::Function *fn = model.getFunction(scc[i]);
                names += fn ? fn->name : "(unknown)";
            }

            out << "[mutual] " << names << "\n";
            for (const auto &fid : scc)
            {
                const CodeListener::Core::Function *fn_loc = model.getFunction(fid);
                if (fn_loc)
                {
                    ctx.reporter.report(CodeListener::Core::DiagnosticLevel::Warning, fn_loc->source_location,
                                        "recursion_check: part of mutual recursion group: " + names);
                }
            }
            found_any = true;
        }
        // possibly directly recursive
        else if (scc.size() == 1)
        {
            // check whether this function has an outgoing call to itself
            const auto fid = scc[0];
            auto it = cg.nodes.find(fid);
            if (it == cg.nodes.end())
            {
                continue;
            }

            for (const auto &edge : it->second.outgoing_calls)
            {
                if (edge.callee.has_value() && *edge.callee == fid)
                {
                    const CodeListener::Core::Function *fn = model.getFunction(fid);
                    const std::string name = fn ? fn->name : "(unknown)";

                    out << "[direct] " << name << "\n";
                    ctx.reporter.report(CodeListener::Core::DiagnosticLevel::Warning, fn->source_location,
                                        "recursion_check: direct recursion in function '" + name + "'");
                    found_any = true;
                    break;
                }
            }
        }
    }

    if (!found_any)
    {
        out << "[none]   no recursion detected\n";
    }

    return true;
}

/** Native analyzer API descriptor for the recursion_check plugin. */
static const cl_native_analyzer_api_t recursion_check_api = {
    CL_NATIVE_API_VERSION,
    recursion_analyze,
};

extern "C" CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void)
{
    return &recursion_check_api;
}
