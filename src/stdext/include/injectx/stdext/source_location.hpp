// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#if __has_include(<source_location>)

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
