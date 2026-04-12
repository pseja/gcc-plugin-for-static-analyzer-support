/**
 * Callgraph DOT demo analyzer.
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
 *
 * or:
 *   make callgraph FILE=foo.c [ARGS="callgraph.dot"]
 */

#include <fstream>
#include <string>

#include <cl_native_analyzer_api.h>

#include "CallGraph.hpp"
#include "CodeModel.hpp"
#include "FunctionId.hpp"

using namespace CodeListener;

static void callgraph_analyze(const Core::CodeModel &model, const char *args)
{
    const std::string outpath = (args && args[0] != '\0') ? args : "callgraph.dot";

    const auto &cg = AnnotationServices::CallGraph::build(model);

    std::ofstream out(outpath);
    if (!out.is_open())
    {
        fprintf(stderr, "callgraph_dot: cannot open output file '%s'\n", outpath.c_str());
        return;
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
}

static const cl_native_analyzer_api_t callgraph_dot_api = {
    CL_NATIVE_API_VERSION,
    callgraph_analyze,
};

extern "C" CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void)
{
    return &callgraph_dot_api;
}
