#ifndef CL_ANALYZER_API_H
#define CL_ANALYZER_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    struct cl_code_listener;

#define CL_ANALYZER_API_VERSION 1

#if defined(_WIN32) || defined(__CYGWIN__)
#define CL_ANALYZER_EXPORT __declspec(dllexport)
#else
#define CL_ANALYZER_EXPORT __attribute__((__visibility__("default")))
#endif

    typedef struct
    {
        /**
         * Must equal CL_ANALYZER_API_VERSION. Checked by the plugin at load
         * time to detect binary-incompatible mismatches.
         */
        int api_version;

        /**
         * Create a fresh listener for one compilation unit.
         *
         * @param args         Value passed via -fplugin-arg-<plugin>-args=VALUE.
         *                     May be NULL or empty string.
         * @param plugin_name  Full path of the calling GCC plugin shared library,
         *                     e.g. "/build/libcl_gcc.so". Used so diagnostics
         *                     can be tagged with "[-fplugin=libcl_gcc.so]".
         *                     May be NULL.
         *
         * @return             A heap-allocated listener. The plugin will call
         *                     listener->destroy() when the compilation finishes.
         *                     Return NULL to signal initialisation failure.
         */
        struct cl_code_listener *(*create)(const char *args, const char *plugin_name);

    } cl_analyzer_api_t;

    /**
     * Entry point that every analyzer shared library must export.
     *
     * The symbol must be visible (not hidden by -fvisibility=hidden).
     * Use the CL_ANALYZER_EXPORT macro on the definition.
     */
    CL_ANALYZER_EXPORT const cl_analyzer_api_t *cl_get_analyzer_api(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CL_ANALYZER_API_H */
