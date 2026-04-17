/**
 * JSON IR dump demo analyzer.
 *
 * Implements the native CodeListener analyzer API (cl_native_analyzer_api.h).
 * When loaded via -fplugin-arg-libcl_gcc-load-analyzer=libcl_json_dump.so
 * it writes the full CodeModel as a JSON file.
 *
 * Output path: derived from the analyzer args, defaulting to "ir_dump.json".
 *
 * Usage example:
 *   gcc -fplugin=libcl_gcc.so \
 *       -fplugin-arg-libcl_gcc-load-analyzer=libcl_json_dump.so \
 *       -fplugin-arg-libcl_gcc-args=output.json \
 *       -o /dev/null -S foo.c
 * or:
 *   make json FILE=foo.c [ARGS="output.json"]
 */

#include <string>

#include <cl_native_analyzer_api.h>

#include "CodeModel.hpp"
#include "JSONExporter.hpp"

using namespace CodeListener;

static void json_dump_analyze(const Core::CodeModel &model, AnalysisContext &ctx, const char *args)
{
    const std::string outpath = (args && args[0] != '\0') ? args : "ir_dump.json";
    ctx.exportJson(model, outpath);
}

static const cl_native_analyzer_api_t json_dump_api = {
    CL_NATIVE_API_VERSION,
    json_dump_analyze,
};

extern "C" CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void)
{
    return &json_dump_api;
}
