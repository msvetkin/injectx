# SPDX-FileCopyrightText: Copyright 2025 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_test_executable(fail)
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

if (MSVC)
  set_tests_properties(${injectx_test_target}-output
    PROPERTIES
      PASS_REGULAR_EXPRESSION "failure was caused by evaluating a throw sub-expression"
  )
else()
  set_tests_properties(${injectx_test_target}-output
    PROPERTIES
      PASS_REGULAR_EXPRESSION "condition failted: 'expected to be true'"
  )
endif()

add_injectx_test(success)
