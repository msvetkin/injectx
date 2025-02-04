# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_subdirectory(concepts)
add_subdirectory(coro)
add_subdirectory(details)
add_subdirectory(expects)
add_subdirectory(monadics)
add_subdirectory(ranges/views)
add_subdirectory(static_assert)

add_injectx_test(coroutine)
add_injectx_test(expected)
add_injectx_test(expected_monadics)
add_injectx_test(function_traits)
add_injectx_test(generator)
add_injectx_test(source_location)
add_injectx_test(static_format)
add_injectx_test(static_map)
add_injectx_test(static_string)
add_injectx_test(type_name)
