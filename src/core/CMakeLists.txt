# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_module(core)

target_sources(${injectx_module_target}
  PRIVATE
    include/injectx/core/bundle.hpp
    include/injectx/core/dependency_container.hpp
    include/injectx/core/dependency_info.hpp
    include/injectx/core/launch.hpp
    include/injectx/core/manifest.hpp
    include/injectx/core/module.hpp
    include/injectx/core/setup_concepts.hpp
    include/injectx/core/setup_task.hpp
    include/injectx/core/setup_traits.hpp

    src/launch.cpp
)

target_link_libraries(${injectx_module_target}
  INTERFACE
    injectx::stdext
)
