# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_module(stdext)

find_package(fmt CONFIG REQUIRED)

target_sources(${injectx_module_target}
  PRIVATE
    include/injectx/stdext/concepts/is_any_of.hpp
    include/injectx/stdext/coro/concepts.hpp
    include/injectx/stdext/coro/promise.hpp
    include/injectx/stdext/coro/task.hpp
    include/injectx/stdext/details/source_location.hpp
    include/injectx/stdext/monadics/and_then.hpp
    include/injectx/stdext/monadics/or_else.hpp
    include/injectx/stdext/monadics/pipe.hpp
    include/injectx/stdext/monadics/transform.hpp
    include/injectx/stdext/monadics/transform_error.hpp
    include/injectx/stdext/ranges/views/enumerate.hpp
    include/injectx/stdext/coroutine.hpp
    include/injectx/stdext/expected.hpp
    include/injectx/stdext/expects.hpp
    include/injectx/stdext/function_traits.hpp
    include/injectx/stdext/generator.hpp
    include/injectx/stdext/monadics.hpp
    include/injectx/stdext/source_location.hpp
    include/injectx/stdext/static_format.hpp
    include/injectx/stdext/static_string.hpp
    include/injectx/stdext/type_name.hpp

    src/coro/promise.cpp
    src/expects.cpp
)

if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_definitions(${injectx_module_target}
    PUBLIC
      $<$<CONFIG:Debug>:INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER>
  )
endif()

target_link_libraries(${injectx_module_target}
  PUBLIC
    fmt::fmt
)
