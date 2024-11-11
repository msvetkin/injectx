# SPDX-FileCopyrightText: Copyright 2023 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_executable(cli)

find_package(fmt CONFIG REQUIRED)

target_sources(${injectx_executable_target}
  PRIVATE
    src/main.cpp
)

target_link_libraries(${injectx_executable_target}
  PRIVATE
    injectx::core
    fmt::fmt
)
