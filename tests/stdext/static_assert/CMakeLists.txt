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
      PASS_REGULAR_EXPRESSION "char{118,97,108,117,101,32,61,32,57,44,32,109,117,115,116,32,98,101,32,49,48,0}"
  )
else()
  set_tests_properties(${injectx_test_target}-output
    PROPERTIES
      PASS_REGULAR_EXPRESSION "value = 9, must be 10"
  )
endif()

add_injectx_test(success)
