cmake_minimum_required(VERSION 3.28)

# ------------------------------------------------------------------------------
# CodeModel integration tests
# ------------------------------------------------------------------------------
# Each test compiles a .c file through the GCC plugin with cl_json_dump, then
# runs a .sh assertion script against the resulting JSON using jq.
#
# To add a new test: 1. Add a .c source file here 2. Add a matching .sh check
# script (same name + ".sh")
# ------------------------------------------------------------------------------

function(add_codemodel_test test_name source_file)
  set(src "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}")
  set(chk "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}.sh")
  set(json "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.json")

  if(NOT EXISTS "${chk}")
    message(
      FATAL_ERROR "Missing check script: ${source_file}.sh\n"
                  "  Add it alongside ${source_file} to register this test.")
  endif()

  add_test(
    NAME "codemodel_${test_name}"
    COMMAND
      bash -c
      "${TARGET_GCC} \
                -S ${src} -o /dev/null \
                -fplugin=$<TARGET_FILE:cl_gcc> \
                -fplugin-arg-libcl_gcc-load-analyzer=$<TARGET_FILE:cl_json_dump> \
                -fplugin-arg-libcl_gcc-args=${json} \
             && bash ${chk} ${json}")
  set_tests_properties("codemodel_${test_name}" PROPERTIES LABELS "codemodel")
endfunction()

# ------------------------------------------------------------------------------
# Primitive types and casts
# ------------------------------------------------------------------------------
add_codemodel_test(integer_variable 002_int.c)
add_codemodel_test(float_to_int_cast 003_float_to_int_cast.c)
add_codemodel_test(double_to_int_cast 004_double_to_int_cast.c)
add_codemodel_test(char_type 005_char.c)
add_codemodel_test(string_literal 006_string.c)
add_codemodel_test(char_array_implicit 007_char_array_implicit_size.c)
add_codemodel_test(char_array_explicit 008_char_array_explicit_size.c)

# ------------------------------------------------------------------------------
# Aggregate types
# ------------------------------------------------------------------------------
add_codemodel_test(struct_type 009_struct.c)
add_codemodel_test(union_type 010_union.c)
add_codemodel_test(nested_structs 021_nested_structs.c)
add_codemodel_test(struct_pointers 022_struct_pointers.c)
add_codemodel_test(bitfields 023_bitfields.c)
add_codemodel_test(complex_initializers 024_complex_initializers.c)
add_codemodel_test(enums 031_enums.c)
add_codemodel_test(anonymous_structs 037_anonymous_structs.c)
add_codemodel_test(complex_types 038_complex_types.c)

# ------------------------------------------------------------------------------
# Control flow
# ------------------------------------------------------------------------------
add_codemodel_test(conditional 011_if.c)
add_codemodel_test(for_loop 012_for.c)
add_codemodel_test(while_loop 013_while.c)
add_codemodel_test(do_while_loop 014_do_while.c)
add_codemodel_test(switch_stmt 015_switch.c)
add_codemodel_test(goto_stmt 026_goto.c)
add_codemodel_test(short_circuit_logic 029_short_circuit_logic.c)
add_codemodel_test(ternary_operator 030_ternary_operator.c)

# ------------------------------------------------------------------------------
# Pointers and memory
# ------------------------------------------------------------------------------
add_codemodel_test(pointers 018_pointers.c)
add_codemodel_test(pointer_arithmetic 019_pointer_arithmetic.c)
add_codemodel_test(multidim_arrays 020_multidim_arrays.c)
add_codemodel_test(void_pointers 033_void_pointers.c)

# ------------------------------------------------------------------------------
# Functions and calls
# ------------------------------------------------------------------------------
add_codemodel_test(function_call 016_function_call.c)
add_codemodel_test(recursion 017_recursion.c)
add_codemodel_test(function_pointers 027_function_pointers.c)
add_codemodel_test(noreturn_abort 028_noreturn_abort.c)
add_codemodel_test(variadic_functions 036_variadic_functions.c)
add_codemodel_test(callgraph 035_callgraph.c)

# ------------------------------------------------------------------------------
# Special / Edge cases
# ------------------------------------------------------------------------------
add_codemodel_test(globals_and_statics 025_globals_and_statics.c)
add_codemodel_test(volatile_var 032_volatile.c)
add_codemodel_test(inline_asm_simple 034_inline_asm_simple.c)
add_codemodel_test(inline_asm 039_inline_asm.c)
