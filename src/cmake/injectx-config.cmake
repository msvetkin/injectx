# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include(CMakeFindDependencyMacro)

find_dependency(fmt)
find_dependency(Microsoft.GSL)
find_dependency(range-v3)

include("${CMAKE_CURRENT_LIST_DIR}/injectx-targets.cmake")
