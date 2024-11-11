# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include_guard(GLOBAL)

find_package(Catch2 CONFIG REQUIRED)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  find_package(injectx CONFIG REQUIRED)
endif()

include(CTest)
include(Catch)

function(_injectx_test_get_subdir_info out_module_name out_subdir_target)
  cmake_path(
    RELATIVE_PATH CMAKE_CURRENT_LIST_DIR
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE test_dir
  )
  cmake_path(HAS_PARENT_PATH test_dir has_parent)

  if (has_parent)
    string(REPLACE "/" ";" test_dir_parts "${test_dir}")
    list(GET test_dir_parts 0 module_name)
    list(JOIN test_dir_parts "-" subdir_target)
  else()
    set(module_name "${test_dir}")
    set(subdir_target "${test_dir}")
  endif()

  string(REPLACE "_" "-" subdir_target "${subdir_target}")

  set(${out_module_name} "${module_name}" PARENT_SCOPE)
  set(${out_subdir_target} "tst-${subdir_target}" PARENT_SCOPE)
endfunction()

function(add_injectx_test_executable test_name)
  _injectx_test_get_subdir_info(module_name subdir_target)
  if (NOT TARGET ${subdir_target})
    add_custom_target(${subdir_target})
  endif()

  set(test_file "${test_name}_test.cpp")
  set(test_target "${subdir_target}-${test_name}")
  string(REPLACE "_" "-" test_target "${test_target}")

  add_executable(${test_target} "${test_file}")
  add_dependencies(${subdir_target} ${test_target})

  target_link_libraries(
    ${test_target}
      PRIVATE
        injectx::${module_name}
  )

  set(injectx_test_target ${test_target} PARENT_SCOPE)
endfunction()

# sets all nessary default things
function(add_injectx_test test_name)
  add_injectx_test_executable(${test_name})

  target_link_libraries(${injectx_test_target}
    PRIVATE
      Catch2::Catch2WithMain
  )

  catch_discover_tests(${injectx_test_target}
    TEST_PREFIX "${injectx_test_target}-"
    ${ARGN}
  )

  set(injectx_test_target ${injectx_test_target} PARENT_SCOPE)
endfunction()
