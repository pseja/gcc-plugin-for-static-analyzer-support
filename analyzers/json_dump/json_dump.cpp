/**
 * @file json_dump.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief JSON IR dump demo analyzer.
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

#include <string>

#include <cl_native_analyzer_api.h>

#include "CodeModel.hpp"
#include "JSONExporter.hpp"

using namespace CodeListener;

static bool json_dump_analyze(const Core::CodeModel &model, AnalysisContext &ctx, const char *args)
{
    const std::string outpath = (args && args[0] != '\0') ? args : "ir_dump.json";
    ctx.exportJson(model, outpath);
    return true;
}

static const cl_native_analyzer_api_t json_dump_api = {
    CL_NATIVE_API_VERSION,
    json_dump_analyze,
};

extern "C" CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void)
{
    return &json_dump_api;
}
