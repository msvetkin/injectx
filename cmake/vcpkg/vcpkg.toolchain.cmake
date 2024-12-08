# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include_guard(GLOBAL)

cmake_minimum_required(VERSION 3.25)

get_property(IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE)

if(IN_TRY_COMPILE)
  return()
endif()

unset(IN_TRY_COMPILE)

include(${CMAKE_CURRENT_LIST_DIR}/bootstrap/vcpkg-config.cmake)

vcpkg_configure(
  CACHE_DIR_NAME injectx
  REPO https://github.com/microsoft/vcpkg.git
  REF b2cb0da531c2f1f740045bfe7c4dac59f0b2b69c # release 2024.11.16
)

include($CACHE{_VCPKG_TOOLCHAIN_FILE})
