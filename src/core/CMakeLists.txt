# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

add_injectx_module(core)

target_sources(${injectx_module_target}
  PRIVATE
    include/injectx/core/version.hpp
    src/version.cpp
)
