cmake_minimum_required(VERSION 3.28)
project(cl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET_VERSION "12")
set(TARGET_GCC "gcc-${TARGET_VERSION}" CACHE STRING "The GCC compiler executable to build the plugin for")

execute_process(
    COMMAND ${TARGET_GCC} -print-file-name=plugin
    OUTPUT_VARIABLE GCC_PLUGIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if("${GCC_PLUGIN_DIR}" STREQUAL "")
    message(FATAL_ERROR "Could not find GCC plugin directory. Install `${TARGET_GCC}-plugin-dev` and `g++-${TARGET_VERSION}`")
endif()

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

message(STATUS "Target GCC: ${TARGET_GCC}")
message(STATUS "GCC Plugin Directory: ${GCC_PLUGIN_DIR}")

set(GCC_PLUGIN_INCLUDE_DIR "${GCC_PLUGIN_DIR}/include")

set(SOURCES
    src/adapters/gcc/register_plugin.cpp
    src/adapters/gcc/PluginContext.cpp
    src/adapters/gcc/PluginArgs.cpp
    src/adapters/gcc/Pass.cpp
    src/adapters/gcc/GCCDiagnosticReporter.cpp
    src/adapters/gcc/GCCAdapter.cpp
    src/core/CodeModel.cpp
    src/core/SourceLocation.cpp
    src/exporters/JSONExporter.cpp
    src/exporters/DOTExporter.cpp
)

add_library(cl SHARED ${SOURCES})

target_include_directories(cl PRIVATE ${GCC_PLUGIN_INCLUDE_DIR} src/adapters/gcc src/core src/annotation_services src/exporters)

target_link_libraries(cl PRIVATE nlohmann_json::nlohmann_json)

target_compile_options(cl PRIVATE -Wall -Wextra -Werror -pedantic -fno-rtti)

set_target_properties(cl PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)
