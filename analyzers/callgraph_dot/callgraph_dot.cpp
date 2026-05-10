/**
 * @file callgraph_dot.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Callgraph DOT demo analyzer.
 *
 * Implements the native CodeListener analyzer API (cl_native_analyzer_api.h).
 * When loaded via -fplugin-arg-libcl_gcc-load-analyzer=libcl_callgraph_dot.so
 * it writes a Graphviz DOT file showing the call graph of the translation unit.
 *
 * Output path: derived from the analyzer args, defaulting to "callgraph.dot".
 *
 * Usage example:
 *   gcc -fplugin=libcl_gcc.so \
 *       -fplugin-arg-libcl_gcc-load-analyzer=libcl_callgraph_dot.so \
 *       -fplugin-arg-libcl_gcc-args=callgraph.dot \
 *       -o /dev/null -S foo.c
 * or:
 *   make callgraph FILE=foo.c [ARGS="callgraph.dot"]
 *
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

#include <fstream>
#include <string>

#include <cl_native_analyzer_api.h>

#include "CallGraph.hpp"
#include "CodeModel.hpp"
#include "FunctionId.hpp"

using namespace CodeListener;

/**
 * Run the callgraph_dot analysis: emit the call graph as a Graphviz DOT file.
 *
 * @param model  Fully populated CodeModel for the translation unit.
 * @param ctx    Services available to the analyzer (reporter, annotation cache).
 * @param args   Output file path override; defaults to "callgraph.dot" when NULL or empty.
 *
 * @return true on success; false if the output file could not be opened.
 */
static bool callgraph_analyze(const Core::CodeModel &model, AnalysisContext &ctx, const char *args)
{
    const std::string outpath = (args && args[0] != '\0') ? args : "callgraph.dot";

    const auto &cg = ctx.analysis_manager.getAnnotation<AnnotationServices::CallGraph>(model);

    std::ofstream out(outpath);
    if (!out.is_open())
    {
        ctx.reporter.report(Core::DiagnosticLevel::Error, "callgraph_dot: cannot open output file '" + outpath + "'");
        return false;
    }

    out << "digraph callgraph {\n";
    out << "    rankdir=LR;\n";
    out << "    node [shape=box fontname=\"monospace\"];\n\n";

    // emit nodes
    for (const auto &[fid, node] : cg.nodes)
    {
        const Core::Function *fn = model.getFunction(fid);
        const std::string &name = fn ? fn->name : "(unknown)";
        out << "    \"" << name << "\";\n";
    }

    out << "\n";

    // emit edges
    for (const auto &[fid, node] : cg.nodes)
    {
        const Core::Function *fn = model.getFunction(fid);
        const std::string &caller = fn ? fn->name : "(unknown)";
        for (const AnnotationServices::CallGraphEdge &edge : node.outgoing_calls)
        {
            if (!edge.callee.has_value())
            {
                // indirect call (function pointer)
                out << "    \"" << caller << "\" -> \"(indirect)\";\n";
                continue;
            }
            const Core::Function *callee_fn = model.getFunction(*edge.callee);
            const std::string &callee = callee_fn ? callee_fn->name : "(unknown)";
            out << "    \"" << caller << "\" -> \"" << callee << "\";\n";
        }
    }

    // annotate roots/leaves
    if (!cg.roots.empty())
    {
        out << "\n    // roots (no callers)\n";
        out << "    { rank=source;";
        for (const Core::FunctionId &fid : cg.roots)
        {
            const Core::Function *fn = model.getFunction(fid);
            out << " \"" << (fn ? fn->name : "(unknown)") << "\";";
        }
        out << " }\n";
    }

    out << "}\n";
    return true;
}

/** Native analyzer API descriptor for the callgraph_dot plugin. */
static const cl_native_analyzer_api_t callgraph_dot_api = {
    CL_NATIVE_API_VERSION,
    callgraph_analyze,
};

extern "C" CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void)
{
    return &callgraph_dot_api;
}
