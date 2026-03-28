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

# shared library for pure library with no dependencies on GCC internals 
file(GLOB_RECURSE CORE_SOURCES src/core/*.cpp src/exporters/*.cpp src/annotation_services/*.cpp)

add_library(cl_core SHARED ${CORE_SOURCES})

target_include_directories(cl_core PUBLIC src/core src/exporters src/annotation_services)

target_link_libraries(cl_core PUBLIC nlohmann_json::nlohmann_json)

target_compile_options(cl_core PRIVATE -g3 -O0 -shared -fPIC -fno-rtti -Wall -Wextra -Werror -pedantic)

set_target_properties(cl_core PROPERTIES CXX_STANDARD 23)

# shared library for the plugin itself, which depends on GCC internals
file(GLOB_RECURSE GCC_SOURCES src/adapters/gcc/*.cpp src/adapters/*.cpp)

# link original predator libraries
add_library(predator_old STATIC IMPORTED)
set_target_properties(predator_old PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/predator/sl_build/libpredator.a)
add_library(cl_old STATIC IMPORTED)
set_target_properties(cl_old PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/predator/cl_build/libcl.a)

add_library(cl_gcc SHARED ${GCC_SOURCES})

target_include_directories(cl_gcc PRIVATE ${GCC_PLUGIN_INCLUDE_DIR} src/adapters/gcc)

target_link_libraries(cl_gcc PRIVATE cl_core cl_old predator_old)

target_compile_options(cl_gcc PRIVATE -g3 -O0 -shared -fPIC -fno-rtti -Wall -Wextra -Werror -pedantic)

set_target_properties(cl_gcc PROPERTIES CXX_STANDARD 23)
