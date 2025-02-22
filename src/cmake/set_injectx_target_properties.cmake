# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include_guard(GLOBAL)

function(set_injectx_target_properties target type)
  target_compile_features(${target} ${type} cxx_std_20)
  set_target_properties(${target}
    PROPERTIES
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN ON
  )

  target_compile_options(${target}
    ${type}
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>>:-stdlib=libc++>
  )

  target_link_options(${target}
    ${type}
      $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>>:-stdlib=libc++ -lc++abi>
  )

  if (NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    target_compile_options(${target} ${type}
      $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4251>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    )
  endif()
endfunction()
