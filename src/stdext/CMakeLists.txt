# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_module(stdext INTERFACE)

find_package(fmt CONFIG REQUIRED)

target_sources(${injectx_module_target}
  PRIVATE
    include/injectx/stdext/concepts/is_any_of.hpp
    include/injectx/stdext/coro/concepts.hpp
    include/injectx/stdext/details/source_location.hpp
    include/injectx/stdext/coroutine.hpp
    include/injectx/stdext/source_location.hpp
)

target_link_libraries(${injectx_module_target}
  INTERFACE
    fmt::fmt
)
