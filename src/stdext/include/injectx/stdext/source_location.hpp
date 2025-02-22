// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <version>

#if defined(__cpp_lib_source_location) \
    && !(defined(__clang__) && defined(__APPLE__) && __clang_major__ == 15)

#  include <source_location>

namespace injectx::stdext {

using source_location = std::source_location;

}  // namespace injectx::stdext

#else

#  include "injectx/stdext/details/source_location.hpp"

namespace injectx::stdext {

using source_location = details::_source_location::source_location;

}  // namespace injectx::stdext

#endif
