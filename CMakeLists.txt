cmake_minimum_required(VERSION 3.28)
project(cl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

set(TARGET_VERSION "12")
set(TARGET_GCC
    "gcc-${TARGET_VERSION}"
    CACHE STRING "The GCC compiler executable to build the plugin for")

option(WITH_PREDATOR
       "Build predator (libsl_analyzer.so) and its regression tests" ON)

execute_process(
  COMMAND ${TARGET_GCC} -print-file-name=plugin
  OUTPUT_VARIABLE GCC_PLUGIN_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if("${GCC_PLUGIN_DIR}" STREQUAL "")
  message(
    FATAL_ERROR
      "Could not find GCC plugin directory. Install `${TARGET_GCC}-plugin-dev` and `g++-${TARGET_VERSION}`"
  )
endif()

include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

message(STATUS "Target GCC: ${TARGET_GCC}")
message(STATUS "GCC Plugin Directory: ${GCC_PLUGIN_DIR}")

set(GCC_PLUGIN_INCLUDE_DIR "${GCC_PLUGIN_DIR}/include")

# shared library for pure library with no dependencies on GCC internals
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp src/exporters/*.cpp
     src/annotation_services/*.cpp)

add_library(cl_core SHARED ${CORE_SOURCES})

target_include_directories(cl_core PUBLIC src/core src/exporters
                                          src/annotation_services include)

target_link_libraries(cl_core PUBLIC nlohmann_json::nlohmann_json)

target_compile_options(
  cl_core
  PRIVATE -g3
          -O0
          -shared
          -fPIC
          -fno-rtti
          -Wall
          -Wextra
          -Werror
          -pedantic)

set_target_properties(cl_core PROPERTIES CXX_STANDARD 23)

# shared library for the plugin itself, which depends on GCC internals
file(GLOB_RECURSE GCC_SOURCES src/adapters/gcc/*.cpp src/adapters/*.cpp)

add_library(cl_gcc SHARED ${GCC_SOURCES})

target_include_directories(
  cl_gcc PRIVATE ${GCC_PLUGIN_INCLUDE_DIR} src/adapters/gcc
                 src/adapters/predator include)

target_link_libraries(cl_gcc PRIVATE cl_core dl)

target_compile_options(
  cl_gcc
  PRIVATE -g3
          -O0
          -shared
          -fPIC
          -fno-rtti
          -Wall
          -Wextra
          -Werror
          -pedantic)

set_target_properties(cl_gcc PROPERTIES CXX_STANDARD 23)

# demo native analyzers each analyzer is a small shared library that implements
# cl_native_analyzer_api.h load one via:
# -fplugin-arg-libcl_gcc-load-analyzer=<path_to_lib>.so
# -fplugin-arg-libcl_gcc-args=<output_file>
function(add_cl_analyzer target source)
  add_library(${target} SHARED ${source})
  target_include_directories(${target} PRIVATE include src/core src/exporters
                                               src/annotation_services)
  target_link_libraries(${target} PRIVATE cl_core)
  target_compile_options(
    ${target}
    PRIVATE -fvisibility=default
            -shared
            -fPIC
            -fno-rtti
            -Wall
            -Wextra
            -Werror)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 23)
endfunction()

add_cl_analyzer(cl_callgraph_dot analyzers/callgraph_dot/callgraph_dot.cpp)
add_cl_analyzer(cl_json_dump analyzers/json_dump/json_dump.cpp)
add_cl_analyzer(cl_recursion_check
                analyzers/recursion_check/recursion_check.cpp)

# standalone tools (cl_merge, cl_analyze)
function(add_cl_tool target)
  add_executable(${target} src/tools/${target}/main.cpp)
  target_include_directories(${target} PRIVATE include src/core src/exporters
                                               src/annotation_services)
  target_link_libraries(${target} PRIVATE cl_core ${CMAKE_DL_LIBS})
  target_compile_options(${target} PRIVATE -Wall -Wextra -Werror -fno-rtti)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 23)
endfunction()

add_cl_tool(cl_merge)
add_cl_tool(cl_analyze)

# predator integration when WITH_PREDATOR=ON: 1. apply
# build-aux/cl_gcc_support.patch to add sl_analyzer_shim.cc and the new-plugin
# test macros to predator's sl/CMakeLists.txt 2. configure and build
# predator/sl/ as an ExternalProject, which produces libsl_analyzer.so and
# registers the regression tests with CTest
#
# after a top-level build the tests can be run with: ctest --test-dir
# <build>/predator/sl_build -R "^new-plugin-"
if(WITH_PREDATOR)
  find_program(
    GCC_HOST_BIN ${TARGET_GCC} REQUIRED
    DOC "GCC used by predator tests (must match the plugin's TARGET_GCC)")

  set(PREDATOR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/analyzers/predator")
  set(PREDATOR_BUILD "${CMAKE_CURRENT_BINARY_DIR}/analyzers/predator/sl_build")
  set(PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build-aux/cl_gcc_support.patch")

  # replication of what predator's Makefile-based build generates so the
  # CMake-only ExternalProject build can compile version.c
  execute_process(
    COMMAND git log -1 --format=%H
    WORKING_DIRECTORY "${PREDATOR_SRC}"
    OUTPUT_VARIABLE PREDATOR_GIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if(NOT PREDATOR_GIT_SHA1)
    set(PREDATOR_GIT_SHA1 "unknown")
  endif()
  file(WRITE "${PREDATOR_SRC}/cl/version_cl.h"
       "#define CL_GIT_SHA1 \"${PREDATOR_GIT_SHA1}\"\n")
  file(WRITE "${PREDATOR_SRC}/sl/version.h"
       "#define SL_GIT_SHA1 \"${PREDATOR_GIT_SHA1}\"\n")

  set(PREDATOR_CL_BUILD "${PREDATOR_SRC}/cl_build")

  include(ExternalProject)
  ExternalProject_Add(
    predator_cl
    SOURCE_DIR "${PREDATOR_SRC}/cl"
    BINARY_DIR "${PREDATOR_CL_BUILD}"
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DGCC_HOST=${GCC_HOST_BIN}
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target cl --target clgcc
    INSTALL_COMMAND "")

  ExternalProject_Add(
    predator_sl
    SOURCE_DIR "${PREDATOR_SRC}/sl"
    BINARY_DIR "${PREDATOR_BUILD}"
    PATCH_COMMAND patch --forward -p1 -d "${PREDATOR_SRC}" --input
                  "${PATCH_FILE}" || true
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DGCC_HOST=${GCC_HOST_BIN}
               -DCL_GCC_PLUGIN=$<TARGET_FILE:cl_gcc>
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --target sl_analyzer
    INSTALL_COMMAND ""
    DEPENDS cl_gcc predator_cl)

  message(STATUS "Predator build dir: ${PREDATOR_BUILD}")
  message(
    STATUS
      "Run predator tests: ctest --test-dir ${PREDATOR_BUILD} -R \"^new-plugin-\""
  )
endif()

# CodeModel integration tests
add_subdirectory(tests/integration_tests)
# CL-Adapter (gcc-adapter) tests (predator's original test suite with jq
# assertions)
add_subdirectory(tests/cl_adapter_tests)
# cl_analyze pipeline tests (JSONFrontend + NativeAnalyzerBridge end-to-end)
add_subdirectory(tests/cl_analyze_tests)
