# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_subdirectory(concepts)
add_subdirectory(coro)
add_subdirectory(details)
add_subdirectory(expects)

add_injectx_test(coroutine)
add_injectx_test(source_location)
