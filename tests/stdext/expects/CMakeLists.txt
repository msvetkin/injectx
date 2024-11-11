# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_test_executable(fail_constexpr)
set_target_properties(${injectx_test_target}
  PROPERTIES
    EXCLUDE_FROM_ALL TRUE
)
add_test(
  NAME ${injectx_test_target}-exit-code
  COMMAND
    ${CMAKE_COMMAND}
      --build ${CMAKE_BINARY_DIR}
      --target ${injectx_test_target}
      --config $<CONFIGURATION>
)
set_tests_properties(${injectx_test_target}-exit-code
  PROPERTIES
    WILL_FAIL TRUE
)

add_test(
  NAME ${injectx_test_target}-output
  COMMAND
    ${CMAKE_COMMAND}
      --build ${CMAKE_BINARY_DIR}
      --target ${injectx_test_target}
      --config $<CONFIGURATION>
)
set_tests_properties(${injectx_test_target}-output
  PROPERTIES
    PASS_REGULAR_EXPRESSION "condition failted: 'expected to be true'"
)

add_injectx_test_executable(fail_runtime)
add_test(
  NAME ${injectx_test_target}-exit-code
  COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:${injectx_test_target}>
)
set_tests_properties(${injectx_test_target}-exit-code
  PROPERTIES
    WILL_FAIL TRUE
)
add_test(
  NAME ${injectx_test_target}-output
  COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:${injectx_test_target}>
)
set(expected_msg
  "condition failed: 'something is wrong' at"
  "expects_fail_runtime_test.cpp:9"
)
set_tests_properties(${injectx_test_target}-output
  PROPERTIES
    PASS_REGULAR_EXPRESSION "${expected_msg}"
)
add_injectx_test(success)
