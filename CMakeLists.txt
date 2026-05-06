cmake_minimum_required(VERSION 3.28)
project(cl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

# GCC plugin builds require a GCC binary and matching plugin headers. The
# minimum supported version is kept explicit so both auto-detection and
# diagnostics can refer to the same baseline.
set(TARGET_GCC_MINIMUM_VERSION "12")
set(TARGET_GCC
    ""
    CACHE
      STRING
      "The GCC compiler executable name or absolute path to build the plugin for. Leave empty to auto-detect a usable GCC >= ${TARGET_GCC_MINIMUM_VERSION}."
)

# resolve a candidate compiler name or path to a canonical executable path
function(resolve_gcc_binary out_var candidate)
  if(IS_ABSOLUTE "${candidate}")
    if(EXISTS "${candidate}")
      file(REAL_PATH "${candidate}" resolved_gcc_bin)
      set(${out_var}
          "${resolved_gcc_bin}"
          PARENT_SCOPE)
    else()
      set(${out_var}
          ""
          PARENT_SCOPE)
    endif()
    return()
  endif()

  find_program(resolved_gcc_bin NAMES "${candidate}")
  if(resolved_gcc_bin)
    file(REAL_PATH "${resolved_gcc_bin}" resolved_gcc_bin)
  endif()
  set(${out_var}
      "${resolved_gcc_bin}"
      PARENT_SCOPE)
endfunction()

# inspect one GCC candidate for version support and plugin-header availability
function(
  inspect_gcc_candidate
  candidate
  out_ok
  out_bin
  out_version
  out_plugin_dir
  out_default_include
  out_has_headers
  out_reason)
  resolve_gcc_binary(candidate_bin "${candidate}")
  if(NOT candidate_bin)
    set(${out_ok}
        FALSE
        PARENT_SCOPE)
    set(${out_reason}
        "not found"
        PARENT_SCOPE)
    return()
  endif()

  execute_process(
    COMMAND "${candidate_bin}" -dumpfullversion
    OUTPUT_VARIABLE candidate_version
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  if("${candidate_version}" STREQUAL "")
    execute_process(
      COMMAND "${candidate_bin}" -dumpversion
      OUTPUT_VARIABLE candidate_version
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  endif()

  string(REGEX MATCH "^[0-9]+" candidate_major "${candidate_version}")
  if("${candidate_major}" STREQUAL "")
    set(${out_ok}
        FALSE
        PARENT_SCOPE)
    set(${out_reason}
        "could not determine GCC version"
        PARENT_SCOPE)
    return()
  endif()

  if("${candidate_major}" VERSION_LESS "${TARGET_GCC_MINIMUM_VERSION}")
    set(${out_ok}
        FALSE
        PARENT_SCOPE)
    set(${out_reason}
        "version ${candidate_version} is below the minimum supported GCC ${TARGET_GCC_MINIMUM_VERSION}"
        PARENT_SCOPE)
    return()
  endif()

  execute_process(
    COMMAND "${candidate_bin}" -print-file-name=plugin
    RESULT_VARIABLE candidate_plugin_status
    OUTPUT_VARIABLE candidate_plugin_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

  if(NOT candidate_plugin_status EQUAL 0
     OR "${candidate_plugin_dir}" STREQUAL ""
     OR "${candidate_plugin_dir}" STREQUAL "plugin")
    set(${out_ok}
        FALSE
        PARENT_SCOPE)
    set(${out_reason}
        "could not query the GCC plugin directory"
        PARENT_SCOPE)
    return()
  endif()

  set(candidate_default_include "${candidate_plugin_dir}/include")
  if(EXISTS "${candidate_default_include}/gcc-plugin.h")
    set(candidate_has_headers TRUE)
  else()
    set(candidate_has_headers FALSE)
  endif()

  set(${out_ok}
      TRUE
      PARENT_SCOPE)
  set(${out_bin}
      "${candidate_bin}"
      PARENT_SCOPE)
  set(${out_version}
      "${candidate_version}"
      PARENT_SCOPE)
  set(${out_plugin_dir}
      "${candidate_plugin_dir}"
      PARENT_SCOPE)
  set(${out_default_include}
      "${candidate_default_include}"
      PARENT_SCOPE)
  set(${out_has_headers}
      ${candidate_has_headers}
      PARENT_SCOPE)
  set(${out_reason}
      ""
      PARENT_SCOPE)
endfunction()

# either validate the user-provided TARGET_GCC or auto-detect the first usable
# GCC >= the supported minimum that has matching plugin headers
if(TARGET_GCC)
  inspect_gcc_candidate(
    "${TARGET_GCC}"
    TARGET_GCC_OK
    TARGET_GCC_BIN
    TARGET_GCC_VERSION
    GCC_PLUGIN_DIR
    TARGET_GCC_DEFAULT_INCLUDE
    TARGET_GCC_HAS_HEADERS
    TARGET_GCC_ERROR)
  if(NOT TARGET_GCC_OK)
    message(
      FATAL_ERROR
        "Configured TARGET_GCC `${TARGET_GCC}` is unusable: ${TARGET_GCC_ERROR}"
    )
  endif()

  set(TARGET_GCC_SELECTION "manual")
else()
  set(target_gcc_candidates)

  # prefer explicit versioned GCC binaries starting at the minimum supported
  # version
  foreach(version RANGE ${TARGET_GCC_MINIMUM_VERSION} 20)
    list(APPEND target_gcc_candidates "gcc-${version}")
  endforeach()

  if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    list(APPEND target_gcc_candidates "${CMAKE_C_COMPILER}")
  endif()
  list(APPEND target_gcc_candidates gcc)
  list(REMOVE_DUPLICATES target_gcc_candidates)

  set(target_gcc_rejections)
  foreach(candidate IN LISTS target_gcc_candidates)
    inspect_gcc_candidate(
      "${candidate}"
      candidate_ok
      candidate_bin
      candidate_version
      candidate_plugin_dir
      candidate_default_include
      candidate_has_headers
      candidate_error)
    if(candidate_ok)
      if(GCC_PLUGIN_INCLUDE_DIR OR candidate_has_headers)
        set(TARGET_GCC_BIN "${candidate_bin}")
        set(TARGET_GCC_VERSION "${candidate_version}")
        set(GCC_PLUGIN_DIR "${candidate_plugin_dir}")
        set(TARGET_GCC_DEFAULT_INCLUDE "${candidate_default_include}")
        set(TARGET_GCC_HAS_HEADERS "${candidate_has_headers}")
        set(TARGET_GCC_SELECTION "auto")
        break()
      endif()

      list(
        APPEND
        target_gcc_rejections
        "${candidate_bin} (version ${candidate_version}) does not provide ${candidate_default_include}/gcc-plugin.h"
      )
    else()
      list(APPEND target_gcc_rejections "${candidate}: ${candidate_error}")
    endif()
  endforeach()

  if(NOT TARGET_GCC_BIN)
    if(target_gcc_rejections)
      string(JOIN "\n  - " target_gcc_rejections_text ${target_gcc_rejections})
      set(target_gcc_rejections_block
          "\nChecked candidates:\n  - ${target_gcc_rejections_text}")
    else()
      set(target_gcc_rejections_block "")
    endif()

    if(GCC_PLUGIN_INCLUDE_DIR)
      message(
        FATAL_ERROR
          "Could not auto-detect a usable GCC >= ${TARGET_GCC_MINIMUM_VERSION}. Set TARGET_GCC to a compiler name or absolute path.${target_gcc_rejections_block}"
      )
    else()
      message(
        FATAL_ERROR
          "Could not auto-detect a usable GCC >= ${TARGET_GCC_MINIMUM_VERSION} with installed plugin headers. Install a matching gcc-<version>-plugin-dev package, or set TARGET_GCC and optionally GCC_PLUGIN_INCLUDE_DIR manually.${target_gcc_rejections_block}"
      )
    endif()
  endif()
endif()

# allow a manual override when the plugin headers come from a locally unpacked
# GCC tree rather than the selected compiler's default plugin include
set(GCC_PLUGIN_INCLUDE_DIR
    ""
    CACHE PATH
          "Path to the GCC plugin header directory containing gcc-plugin.h")

option(WITH_PREDATOR
       "Build predator (libsl_analyzer.so) and its regression tests" ON)

if(NOT GCC_PLUGIN_INCLUDE_DIR)
  set(GCC_PLUGIN_INCLUDE_DIR "${TARGET_GCC_DEFAULT_INCLUDE}")
endif()

if(NOT EXISTS "${GCC_PLUGIN_INCLUDE_DIR}/gcc-plugin.h")
  message(
    FATAL_ERROR
      "Could not find gcc-plugin.h in `${GCC_PLUGIN_INCLUDE_DIR}`. `${TARGET_GCC_BIN} -print-file-name=plugin` may return a plugin directory even when the matching plugin headers are not installed. Install the matching plugin headers for that compiler, or set GCC_PLUGIN_INCLUDE_DIR to a local unpacked header tree."
  )
endif()

# fetch nlohmann/json for the CodeModel JSON import/export pipeline TODO: only
# download if find_package fails
include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

message(STATUS "Target GCC: ${TARGET_GCC}")
message(STATUS "Target GCC Selection: ${TARGET_GCC_SELECTION}")
message(STATUS "Resolved Target GCC: ${TARGET_GCC_BIN}")
message(STATUS "Resolved Target GCC Version: ${TARGET_GCC_VERSION}")
message(STATUS "GCC Plugin Directory: ${GCC_PLUGIN_DIR}")
message(STATUS "GCC Plugin Include Directory: ${GCC_PLUGIN_INCLUDE_DIR}")

# export the resolved GCC choice so the top-level Makefile uses the exact same
# compiler for plugin runs and analyzer helpers
file(
  GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/make-config.mk"
  CONTENT "TARGET_GCC := ${TARGET_GCC_BIN}\nCC := ${TARGET_GCC_BIN}\n")

# gather all source subdirectories
function(collect_subdirectories out_var base_dir)
  set(dirs "${base_dir}")
  file(
    GLOB_RECURSE children
    LIST_DIRECTORIES true
    CONFIGURE_DEPENDS "${base_dir}/*")
  foreach(child IN LISTS children)
    if(IS_DIRECTORY "${child}")
      list(APPEND dirs "${child}")
    endif()
  endforeach()
  list(REMOVE_DUPLICATES dirs)
  set(${out_var}
      "${dirs}"
      PARENT_SCOPE)
endfunction()

# expose both src/ and include/ roots to every target that still relies on the
# current flat include style
collect_subdirectories(CL_SRC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
list(APPEND CL_SRC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

# shared library for pure library with no dependencies on GCC internals
file(
  GLOB_RECURSE
  CORE_SOURCES
  CONFIGURE_DEPENDS
  src/model/*.cpp
  src/analysis/*.cpp
  src/exporters/*.cpp
  src/runtime/*.cpp
  src/frontends/json/*.cpp
  src/bridges/native/*.cpp)

list(FILTER CORE_SOURCES EXCLUDE REGEX ".*/GCCDiagnosticReporter\\.cpp$")

add_library(cl_core SHARED ${CORE_SOURCES})

target_include_directories(cl_core PUBLIC ${CL_SRC_INCLUDE_DIRS})

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
file(GLOB_RECURSE GCC_SOURCES CONFIGURE_DEPENDS src/frontends/gcc/*.cpp
     src/bridges/predator/*.cpp)

list(APPEND GCC_SOURCES src/runtime/GCCDiagnosticReporter.cpp)

add_library(cl_gcc SHARED ${GCC_SOURCES})

target_include_directories(cl_gcc PRIVATE ${GCC_PLUGIN_INCLUDE_DIR}
                                          ${CL_SRC_INCLUDE_DIRS})

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
  target_include_directories(${target} PRIVATE ${CL_SRC_INCLUDE_DIRS})
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
  target_include_directories(${target} PRIVATE ${CL_SRC_INCLUDE_DIRS})
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
  set(GCC_HOST_BIN "${TARGET_GCC_BIN}")

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

# Doxygen documentation target (HTML + LaTeX/PDF)
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
  set(DOXYFILE "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile")
  add_custom_target(
    docs
    COMMAND ${DOXYGEN_EXECUTABLE} "${DOXYFILE}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
  message(
    STATUS
      "Doxygen found - run 'cmake --build build --target docs' to generate docs"
  )
else()
  message(STATUS "Doxygen not found - 'docs' target unavailable")
endif()

# CodeModel integration tests
add_subdirectory(tests/integration_tests)
# CL-Adapter (gcc-adapter) tests (predator's original test suite with jq
# assertions)
add_subdirectory(tests/cl_adapter_tests)
# cl_analyze pipeline tests (JSONFrontend + NativeAnalyzerBridge end-to-end)
add_subdirectory(tests/cl_analyze_tests)
