# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_module(core INTERFACE)

target_sources(${injectx_module_target}
  PRIVATE
    include/injectx/core/dependency_info.hpp
    include/injectx/core/manifest.hpp
    include/injectx/core/setup_concepts.hpp
    include/injectx/core/setup_task.hpp
    include/injectx/core/setup_traits.hpp
)

target_link_libraries(${injectx_module_target}
  INTERFACE
    injectx::stdext
)
