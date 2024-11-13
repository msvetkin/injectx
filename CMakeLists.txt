# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.25)

project(injectx-dev LANGUAGES CXX)

if(WIN32)
  # TODO try TARGET_RUNTIME_DLL_DIRS
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
