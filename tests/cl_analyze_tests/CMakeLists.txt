cmake_minimum_required(VERSION 3.28)

# ------------------------------------------------------------------------------
# cl_analyze pipeline tests
# ------------------------------------------------------------------------------
# Each test compiles a .c file through the GCC plugin (with cl_json_dump) to
# produce a CodeModel JSON file, feeds that JSON to cl_analyze together with a
# specific native analyzer, and runs a .sh assertion script against the
# analyzer's output file.
#
# This exercises the JSONFrontend -> NativeAnalyzerBridge -> IAnalyzer pipeline.
#
# Sources shared with integration tests live in tests/integration_tests/.
# Sources that are specific to this suite live alongside this CMakeLists.txt.
#
# To add a new test: 1. Identify or add the .c source file. 2. Write a .sh
# assertion script in this directory. 3. Call add_cl_analyze_test() below.
# ------------------------------------------------------------------------------

set(INTEGRATION_SRC_DIR "${CMAKE_SOURCE_DIR}/tests/integration_tests")
set(LOCAL_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

function(add_cl_analyze_test test_name source_file analyzer_target check_script
         output_ext)
  if(EXISTS "${LOCAL_SRC_DIR}/${source_file}")
    set(src "${LOCAL_SRC_DIR}/${source_file}")
  else()
    set(src "${INTEGRATION_SRC_DIR}/${source_file}")
  endif()

  set(chk "${CMAKE_CURRENT_SOURCE_DIR}/${check_script}")
  set(json "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.json")
  set(output "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.${output_ext}")

  if(NOT EXISTS "${chk}")
    message(
      FATAL_ERROR
        "Missing check script: tests/cl_analyze_tests/${check_script}\n"
        "  Add it to register this test.")
  endif()

  add_test(
    NAME "cl_analyze_${test_name}"
    COMMAND
      bash -c
      # compile .c to JSON model
      "${TARGET_GCC} \
                -S ${src} -o /dev/null \
                -fplugin=$<TARGET_FILE:cl_gcc> \
                -fplugin-arg-libcl_gcc-load-analyzer=$<TARGET_FILE:cl_json_dump> \
                -fplugin-arg-libcl_gcc-args=${json} \
             && \
             # run cl_analyze with the chosen analyzer
             $<TARGET_FILE:cl_analyze> ${json} \
                --analyzer=$<TARGET_FILE:${analyzer_target}> \
                --args=${output} \
             && \
             # run assertion script
             bash ${chk} ${output}")
  set_tests_properties(
    "cl_analyze_${test_name}"
    PROPERTIES LABELS "cl-analyze" DEPENDS
               "${analyzer_target};cl_gcc;cl_json_dump;cl_analyze")
endfunction()

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

# verify DOT output contains correct edges for the call chain main -> baz -> bar
# -> foo (JSONFrontend + callgraph_dot)
add_cl_analyze_test(callgraph_dot_pipeline 035_callgraph.c cl_callgraph_dot
                    001_callgraph_dot.sh dot)

# verify direct recursion is detected in the factorial function (JSONFrontend +
# recursion_check)
add_cl_analyze_test(recursion_check_direct 017_recursion.c cl_recursion_check
                    002_recursion_check_direct.sh txt)

# verify mutual recursion is detected in the even/odd pair (JSONFrontend +
# recursion_check)
add_cl_analyze_test(recursion_check_mutual mutual_recursion.c
                    cl_recursion_check 003_recursion_check_mutual.sh txt)
