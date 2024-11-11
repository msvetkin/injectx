# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include_guard(GLOBAL)

include(set_injectx_target_properties)

# sets all nessary default things
function(add_injectx_executable executable_name)
  set(executable_target injectx-${executable_name})

  add_executable(${executable_target} ${ARGN})
  set_injectx_target_properties(${executable_target} PRIVATE)

  if (TARGET injectx)
    install(TARGETS ${executable_target} EXPORT injectx-targets)
  endif()

  set(injectx_executable_target ${executable_target} PARENT_SCOPE)
endfunction()
