# SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
# SPDX-License-Identifier: MIT

include_guard(GLOBAL)

include(GNUInstallDirs)

add_library(injectx INTERFACE)
add_library(injectx::injectx ALIAS injectx)
install(TARGETS injectx EXPORT injectx-targets)

export(EXPORT injectx-targets NAMESPACE injectx::)
configure_file("cmake/injectx-config.cmake" "." COPYONLY)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  injectx-config-version.cmake COMPATIBILITY SameMajorVersion
)

install(
  EXPORT injectx-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/injectx
  NAMESPACE injectx::
)

install(
  FILES cmake/injectx-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/injectx-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/injectx
)
