CC := gcc-12

MAKEFLAGS += --no-builtin-rules

PLUGIN_FLAGS = -O0 -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-gen-dot=$*.dot -fplugin-arg-libcl-gen-json=$*.json

BUILD_DIR := build
TEST_DIR := tests/integration_tests

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)

.PHONY: all build dump test test-clean clean help FORCE
.PRECIOUS: %.log %.dot

all: build

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j

Makefile: ;

ifneq ($(filter dump,$(MAKECMDGOALS)),)
%.c: dump
DUMP_FLAGS := -O0 -fdump-tree-all-raw -fdump-tree-cfg-graph -fdump-lang-all -dumpdir dump/
endif

%.c: build FORCE
	rm -f $*.png
	$(CC) $(DUMP_FLAGS) $(PLUGIN_FLAGS) $@ > $*.log 2>&1 || (tail -n 20 $*.log && exit 1)

	if [ -f $*.dot ]; then \
		dot -Tpng $*.dot -o $*.png; \
	fi

dump:
	rm -rf dump
	mkdir -p dump

test: $(TEST_SRCS)

test-clean:
	rm -f $(TEST_DIR)/*.dot $(TEST_DIR)/*.json $(TEST_DIR)/*.png $(TEST_DIR)/*.log

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all         - Builds the project (default)"
	@echo "  test        - Runs tests"
	@echo "  test-clean  - Cleans test artifacts"
	@echo "  build       - Builds the project"
	@echo "  dump %.c    - Generates dump files for the specified .c file"
	@echo "  clean       - Cleans build artifacts"
	@echo "  help        - Shows this help message"

FORCE:
