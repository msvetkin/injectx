// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expects.hpp"

#include <fmt/base.h>
#include <fmt/color.h>

#include <cstdio>
#include <exception>

namespace injectx::stdext::details::_expects {

[[noreturn]] INJECTX_STDEXT_EXPORT void terminate(
    std::string_view msg, stdext::source_location sl) noexcept {
  fmt::print(
      fmt::emphasis::bold | fg(fmt::color::red),
      "condition failed: '{}' at {}:{}:{}", msg, sl.file_name(), sl.line(),
      sl.function_name());
  fmt::print(fmt::text_style(), "\n");
  fflush(stdout);

  std::terminate();
}

}  // namespace injectx::stdext::details::_expects
