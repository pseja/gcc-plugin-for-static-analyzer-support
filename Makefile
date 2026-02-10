BUILD_DIR := build

.PHONY: all

all:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) -j
	gcc-12 -fplugin=$(BUILD_DIR)/libcl.so -fplugin-arg-libcl-verbose=1 test.c
