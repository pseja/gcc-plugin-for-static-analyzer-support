CC := gcc-12
FILENAME := test
FILE := $(FILENAME).c
BUILD_DIR := build

.PHONY: all dump

all:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j
	CL_DOT_FILE=$(FILENAME).dot CL_JSON_FILE=$(FILENAME).json $(CC) -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-verbose=1 $(FILE)

dump:
	rm -rf dump
	mkdir -p dump
	$(CC) -O0 -fdump-tree-all-raw -fdump-tree-cfg-graph -fdump-lang-all -dumpdir dump/ $(FILE)
