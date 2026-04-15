CC := gcc-12
BUILD := build
JOBS := $(shell nproc)

PLUGIN := $(BUILD)/libcl_gcc.so
CALLGRAPH := $(BUILD)/libcl_callgraph_dot.so
JSON_DUMP := $(BUILD)/libcl_json_dump.so
PREDATOR_SRC := analyzers/predator
PREDATOR := $(BUILD)/$(PREDATOR_SRC)/sl_build/libsl_analyzer.so
PRED_INC := $(PREDATOR_SRC)/include/predator-builtins
PRED_TESTS := $(BUILD)/$(PREDATOR_SRC)/sl_build
CL_ADAPT_TESTS := $(PREDATOR_SRC)/cl/tests/gcc-adapter/C

# source file to analyze (required by analyzer targets)
FILE ?=
# passed to -fplugin-arg-libcl_gcc-args= (optional)
ARGS ?=

# MAKEFLAGS += --no-builtin-rules
.PHONY: all build configure callgraph json predator \
        test-predator test-cl-adapter test-codemodel test clean help FORCE

# silently check that FILE was provided before an analyzer target runs
define require_file
	@test -n "$(FILE)" || { \
		echo "Usage: make $@ FILE=path/to/file.c [ARGS=...]"; exit 1; }
endef

# run the plugin with a given analyzer.so and optional extra compiler flags
#   $(1) - path to analyzer.so
#   $(2) - extra CFLAGS
define run_analyzer
	$(CC) -S $(FILE) -o /dev/null \
	  -fplugin=$(PLUGIN) \
	  -fplugin-arg-libcl_gcc-load-analyzer=$(1) \
	  $(if $(ARGS),-fplugin-arg-libcl_gcc-args="$(ARGS)") \
	  $(2)
endef

all: build

# build

## run CMake configuration (no build)
configure:
	cmake -S . -B $(BUILD) -DTARGET_GCC=$(CC)

## configure (if needed) and compile everything
build:
	cmake -S . -B $(BUILD) -DTARGET_GCC=$(CC)
	cmake --build $(BUILD) -j$(JOBS)

## callgraph FILE=<src> [ARGS=<output.dot>]
##   emit a Graphviz call-graph DOT file
##   default output: callgraph.dot
callgraph:
	$(call require_file)
	$(call run_analyzer,$(CALLGRAPH),)

## json FILE=<src> [ARGS=<output.json>]
##   dump the full CodeModel as JSON
##   default output: dump.json
json:
	$(call require_file)
	$(call run_analyzer,$(JSON_DUMP),)

## predator FILE=<src> [ARGS=<predator-args>]
##   run the Predator heap-shape / memory-safety analyzer
##   default ARGS: error_label:ERROR
predator:
	$(call require_file)
	$(call run_analyzer,$(PREDATOR), \
	  -std=gnu99 \
	  -DPREDATOR \
	  -I$(PRED_INC) \
	  -fplugin-arg-libcl_gcc-preserve-ec)

# tests

## run the full Predator regression test suite
test-predator:
	ctest --test-dir $(PRED_TESTS) -R "^new-plugin-" -j$(JOBS) --progress

## run the CL GCC-adapter tests
test-cl-adapter:
	ctest --test-dir $(BUILD) -L cl-adapter -j$(JOBS) --output-on-failure --progress

## run CodeModel tests
test-codemodel:
	ctest --test-dir $(BUILD) -L codemodel -j$(JOBS) --output-on-failure --progress

## run all test suites
test: test-cl-adapter test-predator test-codemodel

# cleanup

## remove the build directory
clean: clean-predator
	rm -rf $(BUILD)

## reset the Predator submodule to a clean state
clean-predator:
	git -C $(PREDATOR_SRC) reset --hard HEAD
	git -C $(PREDATOR_SRC) clean -fdx	
	git submodule update --init --recursive $(PREDATOR_SRC)

# help message
help:
	@echo " Build:"
	@echo "     make build          - configure (if needed) and compile everything"
	@echo "     make configure      - run CMake configuration (no build)"
	@echo "     make clean          - remove the build directory"
	@echo "     make clean-predator - reset the Predator submodule to a clean state"
	@echo ""
	@echo " Analyzers:"
	@echo "     make callgraph FILE=foo.c [ARGS=callgraph.dot]     - emit a Graphviz call-graph DOT file"
	@echo "     make json      FILE=foo.c [ARGS=dump.json]         - dump the full CodeModel as JSON"
	@echo "     make predator  FILE=foo.c [ARGS=error_label:ERROR] - run the Predator heap-shape / memory-safety analyzer"
	@echo ""
	@echo " Tests:"
	@echo "     make test-predator   - run all Predator regression tests"
	@echo "     make test-cl-adapter - run all old CL GCC-adapter C tests via libcl_gcc.so"
	@echo "     make test-codemodel  - run all CodeModel tests"
	@echo "     make test            - run all of the above"
