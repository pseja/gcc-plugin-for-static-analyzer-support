# GCC Plugin for Static Analyzer Support

A middleware GCC plugin (`libcl_gcc.so`) that sits between the GCC compiler and external static analyzers. It collects the compiler's intermediate representation, builds a compiler-independent **CodeModel**, and hands it to any loaded analyzer through a C/C++ ABI.

```txt
GCC compilation
     |
     V
libcl_gcc.so (GCC plugin)
     |
     | builds CodeModel
     |
     V
analyzer ABI
  ├── cl_get_analyzer_api() - legacy/C ABI   (e.g. Predator)
  └── cl_get_native_api()   - native/C++ ABI (e.g. callgraph_dot, recursion_check)

Alternatively, without GCC (multi-TU / whole-program):
     |
     V
cl_merge  <a.json> <b.json> ... -o merged.json
     |
     V
cl_analyze <merged.json> --analyzer=<lib.so> [--args=<string>]
```

## Requirements

| Dependency | Version | Notes |
| --- | --- | --- |
| GCC | 12+ | Auto-detected by default; can also be forced with `TARGET_GCC` |
| `gcc-12-plugin-dev` | - | Example plugin-header package; install the matching `gcc-<version>-plugin-dev` for the GCC you want to use |
| `c++` / `g++` | C++23-capable | Used by CMake to build the C++ code |
| CMake | >= 3.28 | |
| `jq` | any | Required by the shell-based test suites |
| patch | any | Used to apply the Predator shim patch |

### Debian/Ubuntu

```bash
sudo apt install gcc-12 g++-12 gcc-12-plugin-dev cmake jq patch
```

If your system has a newer supported GCC, install the matching package instead, for example `gcc-13-plugin-dev` or `gcc-14-plugin-dev`.

## Getting Started

### 1. Clone with submodules

```bash
git clone --recurse-submodules -b implementation git@github.com:pseja/gcc-plugin-for-static-analyzer-support.git
cd gcc-plugin-for-static-analyzer-support
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

### 2. Check dependencies

```bash
make check-deps
```

This verifies the tools and headers needed for the full build, run, and test workflow, and if required dependency check fails and `dpkg-query` is available prints Debian/Ubuntu package hints.

If you do not plan to build Predator, you can match the check to that configuration:

```bash
make check-deps WITH_PREDATOR=OFF
```

### 3. Configure

```bash
make configure
# or
cmake -B build -DTARGET_GCC=gcc-12
```

When `TARGET_GCC` is left unset, CMake auto-detects the first usable GCC `>= 12` that also provides `gcc-plugin.h` in its plugin include directory.

Optional cmake variables:

| Variable | Default | Description |
| --- | --- | --- |
| `TARGET_GCC` | auto-detected | GCC executable name or absolute path to build for and test against |
| `GCC_PLUGIN_INCLUDE_DIR` | auto-detected | Directory containing `gcc-plugin.h`; use to point at a locally unpacked plugin header tree |
| `WITH_PREDATOR` | `ON` | Build Predator and its regression tests |

If auto-detection picks the wrong compiler on a machine with multiple GCC installs, point it at the exact binary you want:

```bash
make configure TARGET_GCC=/usr/bin/gcc-13
```

### 4. Build

```bash
make build
# or:
cmake --build build -j$(nproc)
```

Artifacts produced in `build/`:

| File | Description |
| --- | --- |
| `libcl_gcc.so` | The GCC plugin (pass this to `-fplugin=`) |
| `libcl_core.a` | Compiler-independent CodeModel library |
| `libcl_callgraph_dot.so` | Analyzer - call-graph in Graphviz DOT |
| `libcl_json_dump.so` | Analyzer - full CodeModel as JSON |
| `libcl_recursion_check.so` | Analyzer - direct/mutual recursion detector |
| `cl_merge` | Standalone tool - merge per-TU JSON files into one model |
| `cl_analyze` | Standalone tool - run a native analyzer on a JSON model |
| `analyzers/predator/sl_build/libsl_analyzer.so` | Predator heap-shape analyzer |

## Running an Analyzer

### Single translation unit (via GCC plugin)

All analyzers are loaded at compile time via GCC plugin arguments:

```bash
gcc-12 -S your_file.c -o /dev/null \
  -fplugin=./build/libcl_gcc.so \
  -fplugin-arg-libcl_gcc-load-analyzer=<path-to-analyzer.so> \
  -fplugin-arg-libcl_gcc-args=<analyzer-specific-args>
```

#### Call-graph in DOT format

Writes a Graphviz DOT file showing which functions call which.

```bash
make callgraph FILE=your_file.c [ARGS=callgraph.dot]
```

#### Full CodeModel as JSON

Dumps the entire CodeModel (functions, basic blocks, types, variables, instructions).

```bash
make json FILE=your_file.c [ARGS=dump.json]
```

#### Recursion detector

Detects direct and mutual recursion using the call-graph annotation. Emits GCC-formatted warnings at the location of the function definition and writes a plain-text report.

```bash
make recursion FILE=your_file.c [ARGS=report.txt]
```

Example report for a file with `even`/`odd` mutual recursion:

```txt
[mutual] even, odd
```

Example report for a file with `factorial` direct recursion:

```txt
[direct] factorial
```

#### Predator - heap-shape / memory-safety analyzer

Predator reports memory leaks, use-after-free, and other heap shape violations.

```bash
make predator FILE=your_file.c
```

> The `-DPREDATOR` flag and the predator-builtins include are required so that `__VERIFIER_error()` and similar builtins are visible to the analyzed code.

### Plugin diagnostic verbosity

By default only `Warning` and above level messages from the plugin itself are
forwarded to GCC's diagnostic engine (analyzer warnings always appear).
Use the `verbose` flag to increase the level:

| Flag | Minimum level shown |
| --- | --- |
| - | `Warning`, `Error`, `Fatal` |
| `-fplugin-arg-libcl_gcc-verbose` | + `Info` (notes like "plugin initialized") |
| `-fplugin-arg-libcl_gcc-verbose=2` | + `Debug` (unknown-location traces, etc.) |

## Running multiple Translation Unit (TU) program analyses

```bash
# 1. Compile each TU to JSON (one GCC invocation per file)
gcc-12 -fplugin=build/libcl_gcc.so \
       -fplugin-arg-libcl_gcc-load-analyzer=build/libcl_json_dump.so \
       -fplugin-arg-libcl_gcc-args=a.json -S a.c -o /dev/null
gcc-12 ... -fplugin-arg-libcl_gcc-args=b.json -S b.c -o /dev/null

# 2. Merge all TUs into one whole-program model
build/cl_merge a.json b.json -o merged.json

# 3. Run any native analyzer on the merged model
build/cl_analyze merged.json --analyzer=build/libcl_callgraph_dot.so --args=cg.dot
build/cl_analyze merged.json --analyzer=build/libcl_recursion_check.so --args=report.txt
```

`cl_analyze` also supports built-in exports without an analyzer:

```bash
build/cl_analyze merged.json --gen-dot=cfg.dot [--gen-dot-verbosity=CLEAN|COMPACT|FULL]
build/cl_analyze merged.json --gen-pp=listing.txt
```

Or use the Makefile abstractions:

```bash
# compile every .c file in DIR to JSON, then merge into DIR/merged.json
make json-multi DIR=<dir>

# generate a DOT CFG from DIR/merged.json and render it to DIR/merged.svg
make dot-multi DIR=<dir> [VERBOSITY=CLEAN|COMPACT|FULL]

# shorthand for json-multi + dot-multi in one step
make merge DIR=<dir>
```

## Running Tests

### Predator regression test suite

```bash
make test-predator
```

### GCC-adapter C tests (46 tests)

Verify the plugin's GCC adapter handles a broad range of C language constructs (types, operators, control flow, standard headers, ...) by compiling each file in `analyzers/predator/cl/tests/gcc-adapter/C/` through `libcl_gcc.so` and checking the resulting JSON.

```bash
make test-cl-adapter
```

### CodeModel integration tests (38 tests)

Compile hand-written C fixtures through the plugin and validate the JSON model with `jq` assertions.

```bash
make test-codemodel
```

### `cl_analyze` pipeline tests (3 tests)

Tests for the `cl_analyze` standalone tool. Each test compiles through the GCC plugin, feeds the resulting JSON to `cl_analyze` with a specific analyzer, and asserts on the analyzer's output:

- `callgraph_dot_pipeline` - DOT output contains correct call edges
- `recursion_check_direct` - `factorial` flagged as `[direct]`
- `recursion_check_mutual` - `even`/`odd` flagged as `[mutual]`

```bash
make test-cl-analyze
```

### All suites at once

```bash
make test
```

## Project Structure

```bash
.
├── analyzers/
│   ├── callgraph_dot/           # analyzer: call-graph DOT exporter
│   ├── json_dump/               # analyzer: CodeModel JSON dumper
│   ├── recursion_check/         # analyzer: direct/mutual recursion detector
│   └── predator/                # Git submodule - Predator analyzer
│       ├── include/predator-builtins/ # verifier built-ins header
│       └── sl/                        # Predator's core
├── build-aux/
│   └── cl_gcc_support.patch     # patch applied to predator/sl/ during build
│                                # (adds sl_analyzer_shim.cc and new-plugin tests)
├── include/
│   ├── cl_analyzer_api.h        # C ABI for legacy cl_code_listener analyzers
│   └── cl_native_analyzer_api.h # C++ ABI for native CodeModel analyzers
├── src/
│   ├── adapters/gcc/            # GCC plugin - Pass, GCCAdapter, PluginContext, ...
│   ├── annotation_services/     # On-demand annotations: CallGraph, ...
│   ├── core/                    # CodeModel, IAnalyzer, IFrontend, NativeAnalyzerBridge, ...
│   └── exporters/               # DOTExporter, PPExporter, JSONExporter, JSONFrontend, ...
└── tests/
    ├── cl_adapter_tests/        # CL GCC-adapter C test suite (46 tests)
    ├── cl_analyze_tests/        # cl_analyze pipeline tests (3 tests)
    └── integration_tests/       # CodeModel integration tests (38 tests)
```

## Analyzer ABI Reference

Two ABIs are supported. A single `.so` may implemet either or both.

### Native ABI - `cl_get_native_api()` (preferred for new analyzers)

Defined in `include/cl_native_analyzer_api.h`.

```cpp
extern "C" CL_ANALYZER_EXPORT
const cl_native_analyzer_api_t *cl_get_native_api(void);
```

The struct the function must return:

```cpp
struct cl_native_analyzer_api_t {
    int api_version; // must equal CL_NATIVE_API_VERSION

    // Return true if analysis succeeded; false if at least one error was found.
    // A false return causes the calling tool to exit with a non-zero status
    bool (*analyze)(const CodeListener::Core::CodeModel &model,
                    CodeListener::AnalysisContext &ctx,
                    const char *args);
};
```

`args` is the value of `-fplugin-arg-libcl_gcc-args=...` (or `--args=` for `cl_analyze`); may be `nullptr`.

`ctx` provides:

- `ctx.reporter` - a `DiagnosticReporter` routed through GCC's diagnostic engine when run as a plugin, or to stderr when run via `cl_analyze`. Warnings emitted here appear at the location of the function definition.
- `ctx.analysis_manager` - an `AnalysisManager` for requesting on-demand annotations (e.g. `CallGraph`).

### Legacy ABI - `cl_get_analyzer_api()` (used by Predator)

Defined in `include/cl_analyzer_api.h`. Wraps the original `struct cl_code_listener *` interface.

```c
extern "C" CL_ANALYZER_EXPORT
const cl_analyzer_api_t *cl_get_analyzer_api(void);
```

The plugin tries the native ABI first. If `cl_get_native_api` is absent it falls back to `cl_get_analyzer_api`. If neither symbol is found the load fails.

## Writing a New Analyzer

1. Create `analyzers/my_analyzer/my_analyzer.cpp`.
2. Implement `cl_get_native_api()`.
3. Add it to `CMakeLists.txt`:

```cmake
add_cl_analyzer(cl_my_analyzer analyzers/my_analyzer/my_analyzer.cpp)
```

Minimal skeleton:

```cpp
#include <fstream>
#include <cl_native_analyzer_api.h>
#include "AnalysisContext.hpp"

static bool my_analysis(const CodeListener::Core::CodeModel &model,
                        CodeListener::AnalysisContext &ctx,
                        const char *args)
{
    // request an annotation (computed once, then cached)
    const auto &cg =
        ctx.analysis_manager.getAnnotation<CodeListener::AnnotationServices::CallGraph>(model);

    // emit a diagnostic visible in the compiler output
    ctx.reporter.report(CodeListener::Core::DiagnosticLevel::Warning, "hello from my_analysis");

    return true; // false signals failure
}

static const cl_native_analyzer_api_t my_api = { CL_NATIVE_API_VERSION, my_analysis };

extern "C" CL_ANALYZER_EXPORT
const cl_native_analyzer_api_t *cl_get_native_api(void) { return &my_api; }
```

Then build and run:

```bash
cmake --build build --target cl_my_analyzer

# via GCC plugin (single TU):
gcc-12 -S your_file.c -o /dev/null \
  -fplugin=./build/libcl_gcc.so \
  -fplugin-arg-libcl_gcc-load-analyzer=./build/libcl_my_analyzer.so \
  -fplugin-arg-libcl_gcc-args=optional_args

# via cl_analyze (JSON model, incl. multi-TU merged models):
build/cl_analyze model.json --analyzer=build/libcl_my_analyzer.so --args=optional_args
```
