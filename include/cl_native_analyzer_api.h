/**
 * @file cl_native_analyzer_api.h
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the native C++ analyzer ABI built around the CodeModel.
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

#ifndef CL_NATIVE_ANALYZER_API_H
#define CL_NATIVE_ANALYZER_API_H

#include <cl_analyzer_api.h>

#ifdef __cplusplus

#include "AnalysisContext.hpp"
#include "CodeModel.hpp"

#define CL_NATIVE_API_VERSION 2

#if defined(_WIN32) || defined(__CYGWIN__)
#define CL_ANALYZER_EXPORT __declspec(dllexport)
#else
#define CL_ANALYZER_EXPORT __attribute__((__visibility__("default")))
#endif

/**
 * Describes the entry points that a native analyzer shared library must export.
 */
struct cl_native_analyzer_api_t
{
    /**
     * Must equal CL_NATIVE_API_VERSION.
     */
    int api_version;

    /**
     * Run the analysis on a complete CodeModel.
     *
     * Called once per compilation unit after GCC finishes.
     *
     * @param model The fully populated CodeModel for the translation unit.
     * @param ctx   Services available to the analyzer.
     * @param args  Value of -fplugin-arg-&lt;plugin&gt;-args=VALUE. May be NULL.
     *
     * @return true if analysis succeeded; false if at least one error was found.
     *         A false return causes the calling tool to exit with a non-zero status.
     */
    bool (*analyze)(const CodeListener::Core::CodeModel &model, CodeListener::AnalysisContext &ctx, const char *args);
};

extern "C"
{
    /**
     * Entry point that every native analyzer shared library must export.
     */
    CL_ANALYZER_EXPORT const cl_native_analyzer_api_t *cl_get_native_api(void);
}

#endif /* __cplusplus */
#endif /* CL_NATIVE_ANALYZER_API_H */
