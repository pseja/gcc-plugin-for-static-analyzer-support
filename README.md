# Implementation

**Author:** Lukáš Pšeja  
**Supervisor:** Dr. Ing. Petr Peringer

## Requirements

| Dependency | Version | Notes |
| --- | --- | --- |
| GCC | 12+ | Auto-detected by default; can also be forced with `TARGET_GCC` |
| `gcc-12-plugin-dev` | - | Install the matching `gcc-<version>-plugin-dev` for the GCC you want to use |
| `c++` / `g++` | C++23-capable | Used by CMake to build the C++ code |
| CMake | >= 3.28 | |
| `jq` | any | Required by the shell-based test suites |
| patch | any | Used to apply the Predator shim patch |

### Debian/Ubuntu

```bash
sudo apt install gcc-12 g++-12 gcc-12-plugin-dev cmake jq patch
```

If your system has a newer supported GCC, install the matching package instead, for example `gcc-13-plugin-dev` or `gcc-14-plugin-dev`.

## Usage

### 1. Clone with submodules

```bash
git clone --recurse-submodules -b implementation git@github.com:pseja/gcc-plugin-for-static-analyzer-support.git implementation
cd implementation
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

### 2. Check dependencies

```bash
make check-deps
# or, without Predator:
make check-deps WITH_PREDATOR=OFF
```

### 3. Configure

```bash
make configure
# or
cmake -B build -DTARGET_GCC=gcc-12
```

Optional CMake variables:

| Variable | Default | Description |
| --- | --- | --- |
| `TARGET_GCC` | auto-detected | GCC executable name or absolute path |
| `GCC_PLUGIN_INCLUDE_DIR` | auto-detected | Directory containing `gcc-plugin.h`; can be used to specify local installation of the plugin headers |
| `WITH_PREDATOR` | `OFF` | Don't build Predator and its regression tests |

### 4. Build

```bash
make build
# or:
cmake --build build -j$(nproc)
```

Artifacts produced in `build/`:

| File | Description |
| --- | --- |
| `libcl_gcc.so` | GCC plugin |
| `libcl_core.a` | Compiler-independent CodeModel library |
| `libcl_callgraph_dot.so` | Analyzer - call-graph DOT exporter |
| `libcl_json_dump.so` | Analyzer - CodeModel JSON dumper |
| `libcl_recursion_check.so` | Analyzer - recursion detector |
| `cl_merge` | Merge per-TU JSON files into one model |
| `cl_analyze` | Run a native analyzer on a JSON model |
| `analyzers/predator/sl_build/libsl_analyzer.so` | Predator analyzer |

## Running an Analyzer

### Single translation unit (via GCC plugin)

```bash
gcc-12 -S your_file.c -o /dev/null \
  -fplugin=./build/libcl_gcc.so \
  -fplugin-arg-libcl_gcc-load-analyzer=<path-to-analyzer.so> \
  -fplugin-arg-libcl_gcc-args=<analyzer-specific-args>
```

Makefile shorthands:

```bash
make callgraph FILE=your_file.c [ARGS=callgraph.dot]
make json      FILE=your_file.c [ARGS=dump.json]
make recursion FILE=your_file.c [ARGS=report.txt]
make predator  FILE=your_file.c
```

### Plugin diagnostic verbosity

| Flag | Minimum level shown |
| --- | --- |
| - | `Warning`, `Error`, `Fatal` |
| `-fplugin-arg-libcl_gcc-verbose` | + `Info` |
| `-fplugin-arg-libcl_gcc-verbose=2` | + `Debug` |

## Running Multiple Translation Units

```bash
# 1. Compile each TU to JSON
gcc-12 -fplugin=build/libcl_gcc.so \
       -fplugin-arg-libcl_gcc-load-analyzer=build/libcl_json_dump.so \
       -fplugin-arg-libcl_gcc-args=a.json -S a.c -o /dev/null
gcc-12 ... -fplugin-arg-libcl_gcc-args=b.json -S b.c -o /dev/null

# 2. Merge into a whole-program model
build/cl_merge a.json b.json -o merged.json

# 3. Run an analyzer
build/cl_analyze merged.json --analyzer=build/libcl_callgraph_dot.so --args=cg.dot
```

`cl_analyze` built-in exports:

```bash
build/cl_analyze merged.json --gen-dot=cfg.dot [--gen-dot-verbosity=CLEAN|COMPACT|FULL]
build/cl_analyze merged.json --gen-pp=listing.txt
```

Makefile abstractions:

```bash
make json-multi DIR=<dir>
make dot-multi  DIR=<dir> [VERBOSITY=CLEAN|COMPACT|FULL]
make merge      DIR=<dir>
```

## Running Tests

```bash
make test            # all suites
make test-predator   # Predator regression suite
make test-cl-adapter # GCC-adapter C tests (46 tests)
make test-codemodel  # CodeModel integration tests (38 tests)
make test-cl-analyze # cl_analyze pipeline tests (3 tests)
```

## Project Structure

```bash
.
├── analyzers/
│   ├── callgraph_dot/           # analyzer: call-graph DOT exporter
│   ├── json_dump/               # analyzer: CodeModel JSON dumper
│   ├── recursion_check/         # analyzer: recursion detector
│   └── predator/                # Git submodule - Predator analyzer
├── build-aux/
│   ├── benchmark.sh             # benchmark suite for comparing old vs new plugin
│   ├── check-deps.sh            # dependency checker (prints apt hints on failure)
│   └── cl_gcc_support.patch     # patch applied to predator/sl/ during build
├── docs/                        # Doxygen documentation
├── include/
│   ├── AnalysisContext.hpp      # public AnalysisContext passed to every analyzer
│   ├── cl_analyzer_api.h        # historical C API (cl_code_listener)
│   └── cl_native_analyzer_api.h # native analyzer API (C-ABI)
├── src/
│   ├── analysis/                # on-demand annotations: CallGraph, SwitchToIf, ...
│   ├── bridges/
│   │   ├── native/              # NativeAnalyzerBridge (native analyzer API)
│   │   └── predator/            # LegacyPredatorBridge + PredatorAdapter
│   ├── exporters/               # DOTExporter, PPExporter, JSONExporter, ModelMerger, ...
│   ├── frontends/
│   │   ├── gcc/                 # GCC plugin: Pass, GCCAdapter, PluginContext, ...
│   │   └── json/                # offline JSON frontend
│   ├── model/                   # CodeModel, entity pools, visitors, IR types
│   ├── runtime/                 # AnalysisContext, diagnostic reporters, stats
│   └── tools/                   # entry points for cl_merge and cl_analyze
└── tests/
    ├── cl_adapter_tests/        # GCC-adapter C test suite (46 tests)
    ├── cl_analyze_tests/        # cl_analyze pipeline tests (3 tests)
    └── integration_tests/       # CodeModel integration tests (38 tests)
```
