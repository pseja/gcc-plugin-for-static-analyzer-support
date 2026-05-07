cmake_minimum_required(VERSION 3.28)

# ------------------------------------------------------------------------------
# CL-Adapter (gcc-adapter) tests
# ------------------------------------------------------------------------------
# Each test compiles a .c file from the predator gcc-adapter test suite through
# the GCC plugin with cl_json_dump, then runs a .sh assertion script against the
# resulting JSON using jq.
#
# Source files directory:  analyzers/predator/cl/tests/gcc-adapter/C/ Check
# scripts directory: tests/cl_adapter_tests/
#
# To add a new test: 1. Identify or add a .c file in the source directory 2. Add
# a matching .sh check script named <source_file>.sh here 3. Call
# add_cl_adapter_test() for it below
# ------------------------------------------------------------------------------

set(CL_ADAPT_SRC_DIR
    "${CMAKE_SOURCE_DIR}/analyzers/predator/cl/tests/gcc-adapter/C")

function(add_cl_adapter_test test_name source_file)
  set(src "${CL_ADAPT_SRC_DIR}/${source_file}")
  set(chk "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}.sh")
  set(json "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.json")

  if(NOT EXISTS "${chk}")
    message(
      FATAL_ERROR
        "Missing check script: tests/cl_adapter_tests/${source_file}.sh\n"
        "  Add it alongside this CMakeLists.txt to register this test.")
  endif()

  add_test(
    NAME "cl_adapter_${test_name}"
    COMMAND
      bash -c
      "${TARGET_GCC_BIN} \
                -S ${src} -o /dev/null \
                -std=gnu89 -O0 -DPREDATOR -DNDEBUG \
                -fplugin=$<TARGET_FILE:cl_gcc> \
                -fplugin-arg-libcl_gcc-load-analyzer=$<TARGET_FILE:cl_json_dump> \
                -fplugin-arg-libcl_gcc-args=${json} \
             && bash ${chk} ${json}")
  set_tests_properties("cl_adapter_${test_name}" PROPERTIES LABELS "cl-adapter")
endfunction()

# ------------------------------------------------------------------------------
# Variables and scope
# ------------------------------------------------------------------------------
add_cl_adapter_test(empty_main 00-Empty-main.c)
add_cl_adapter_test(global_var_no_init
                    01-Global-variable-without-initialization.c)
add_cl_adapter_test(global_var_with_value
                    02-Global-variable-with-initial-value.c)
add_cl_adapter_test(global_var_init_in_main
                    03-Global-variable-with-initialization.c)
add_cl_adapter_test(local_var_no_init
                    04-Local-variable-without-initialization.c)
add_cl_adapter_test(local_var_with_init 05-Local-variable-with-initialization.c)
add_cl_adapter_test(memory_class_modificators 15-Memory-class-modificators.c)
add_cl_adapter_test(type_modificators 16-Type-modificators.c)

# ------------------------------------------------------------------------------
# Types
# ------------------------------------------------------------------------------
add_cl_adapter_test(types_char 06-Types-char.c)
add_cl_adapter_test(types_int 07-Types-int.c)
add_cl_adapter_test(types_real 08-Types-real.c)
add_cl_adapter_test(types_enum 09-Types-enum.c)
add_cl_adapter_test(types_union 10-Types-union.c)
add_cl_adapter_test(types_struct 11-Types-struct.c)
add_cl_adapter_test(types_typedef 13-Types-typedef.c)
add_cl_adapter_test(types_pointers 14-Types-pointers.c)
add_cl_adapter_test(bitfields 31-Bitfields.c)
add_cl_adapter_test(types_complex 44-Types-complex.c)

# ------------------------------------------------------------------------------
# Operators
# ------------------------------------------------------------------------------
add_cl_adapter_test(operator_sizeof 12-Operator-sizeof.c)
add_cl_adapter_test(pointers_ref_deref 17-Pointers-reference-dereference.c)
add_cl_adapter_test(type_casting 18-Type-casting.c)
add_cl_adapter_test(operators_unary 19-Operators-unary.c)
add_cl_adapter_test(operators_binary 20-Operators-binary.c)
add_cl_adapter_test(operators_relational 21-Operators-relational-and-equality.c)
add_cl_adapter_test(operators_logical 22-Operators-logical.c)
add_cl_adapter_test(operators_bitwise 23-Operators-bitwise.c)
add_cl_adapter_test(operators_assignment 24-Operators-assignment.c)
add_cl_adapter_test(operators_ternary_comma 25-Operators-ternary-and-comma.c)
add_cl_adapter_test(pointer_arithmetics 32-Pointer-arithmetics.c)

# ------------------------------------------------------------------------------
# Aggregates, arrays, strings
# ------------------------------------------------------------------------------
add_cl_adapter_test(arrays_definitions 26-Arrays-definitions.c)
add_cl_adapter_test(string_literals 30-String-literals.c)
add_cl_adapter_test(compound_literals 33-Compound-literals.c)

# ------------------------------------------------------------------------------
# Functions and calls
# ------------------------------------------------------------------------------
add_cl_adapter_test(functions_definitions 27-Functions-definitions.c)
add_cl_adapter_test(operators_other_calls 28-Operators-other-plus-calls-types.c)
add_cl_adapter_test(declarations 29-Declarations.c)

# ------------------------------------------------------------------------------
# Control flow
# ------------------------------------------------------------------------------
add_cl_adapter_test(control_flow_if 34-Control-flow-if-conditions.c)
add_cl_adapter_test(control_flow_while 35-Control-flow-while-loop.c)
add_cl_adapter_test(control_flow_do_while 36-Control-flow-do-while-loop.c)
add_cl_adapter_test(control_flow_for 37-Control-flow-for-loop.c)
add_cl_adapter_test(control_flow_switch 38-Control-flow-switch-statement.c)
add_cl_adapter_test(control_flow_break_continue
                    39-Control-flow-break-and-continue.c)
add_cl_adapter_test(control_flow_goto 40-Control-flow-goto.c)

# ------------------------------------------------------------------------------
# Standard headers / compatibility
# ------------------------------------------------------------------------------
add_cl_adapter_test(std_headers_c89 41-Standard-header-files-C89.c)
add_cl_adapter_test(header_setjmp 42-Header-setjmp.c)
add_cl_adapter_test(std_headers_c99 43-Standard-header-files-C99.c)
add_cl_adapter_test(std_headers_c11 45-Standard-header-files-C11.c)
