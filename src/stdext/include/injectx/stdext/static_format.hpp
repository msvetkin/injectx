// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/static_string.hpp"

#include <fmt/compile.h>

#include <utility>

namespace injectx::stdext {

namespace details::_static_format {

template<static_string fmt, auto... args>
[[nodiscard]] constexpr auto format() noexcept {
  constexpr auto fmtCompiled = FMT_COMPILE(fmt.data());
  constexpr auto storageSize = fmt::formatted_size(fmtCompiled, args...);
  char storage[storageSize + 1]{};
  fmt::format_to(storage, fmtCompiled, args...);
  return static_string{storage};
}

template<auto fmt, auto... args>
inline constexpr auto formatted = format<fmt, args...>();

}  // namespace details::_static_format

template<static_string fmt, auto... args>
[[nodiscard]] consteval auto static_format() noexcept {
  return std::string_view{details::_static_format::formatted<fmt, args...>};
}

template<static_string fmt, auto... args>
[[nodiscard]] consteval auto static_format(std::in_place_t) noexcept {
  return details::_static_format::formatted<fmt, args...>;
}

}  // namespace injectx::stdext
