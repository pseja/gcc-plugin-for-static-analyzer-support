#ifndef CL_NATIVE_ANALYZER_API_H
#define CL_NATIVE_ANALYZER_API_H

#include <cl_analyzer_api.h>

#ifdef __cplusplus

#include "AnalysisContext.hpp"
#include "CodeModel.hpp"

#define CL_NATIVE_API_VERSION 2

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
     * @param args  Value of -fplugin-arg-<plugin>-args=VALUE. May be NULL.
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
