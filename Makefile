CC := gcc-12
FILENAME := test
FILE := $(FILENAME).c
BUILD_DIR := build

.PHONY: all dump

all:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j
	$(CC) -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-gen-dot=$(FILENAME).dot -fplugin-arg-libcl-gen-json=$(FILENAME).json $(FILE)
# 	$(CC) -wrapper gdb,--args -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-gen-dot=$(FILENAME).dot -fplugin-arg-libcl-gen-json=$(FILENAME).json $(FILE)

dump:
	rm -rf dump
	mkdir -p dump
	$(CC) -O0 -fdump-tree-all-raw -fdump-tree-cfg-graph -fdump-lang-all -dumpdir dump/ $(FILE)
