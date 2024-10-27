// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/static_string.hpp"

#include <fmt/compile.h>

namespace injectx::stdext {

namespace details::_static_format {

template<static_string fmt, auto... args>
[[nodiscard]] constexpr auto format() noexcept {
  constexpr auto fmtCompiled = FMT_COMPILE(fmt.data());
  constexpr auto storageSize = fmt::formatted_size(fmtCompiled, args...);
  std::array<char, storageSize + 1> storage{0};
  fmt::format_to(storage.data(), fmtCompiled, args...);
  return storage;
}

template<auto fmt, auto... args>
inline constexpr auto formatted = format<fmt, args...>();

}  // namespace details::_static_format

template<static_string fmt, auto... args>
[[nodiscard]] consteval std::string_view static_format() noexcept {
  return {details::_static_format::formatted<fmt, args...>.data()};
}

}  // namespace injectx::stdext
