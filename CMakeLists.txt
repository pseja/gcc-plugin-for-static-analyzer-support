cmake_minimum_required(VERSION 3.28)
project(cl)

execute_process(
    # TODO: make this compatible with gcc-12+
    COMMAND gcc-12 -print-file-name=plugin
    OUTPUT_VARIABLE GCC_PLUGIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(GCC_PLUGIN_INCLUDE_DIR "${GCC_PLUGIN_DIR}/include")

add_library(cl SHARED src/adapters/gcc/register_plugin.cpp)

target_include_directories(cl PRIVATE ${GCC_PLUGIN_INCLUDE_DIR})

set_target_properties(cl PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
