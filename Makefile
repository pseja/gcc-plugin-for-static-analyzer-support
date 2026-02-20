CC := gcc-12
FILE := test.c
BUILD_DIR := build

.PHONY: all dump

all:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j
	$(CC) -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-verbose=1 $(FILE)

dump:
	rm -rf dump
	mkdir -p dump
	$(CC) -O0 -fdump-tree-all-raw -fdump-tree-cfg-graph -fdump-lang-all -dumpdir dump/ $(FILE)
