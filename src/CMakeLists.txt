# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.25)

project(injectx VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(injectx-install-targets)
include(add_injectx_module)
include(add_injectx_executable)

add_subdirectory(core)
add_subdirectory(stdext)
