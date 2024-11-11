// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/export_macro.hpp"
#include "injectx/stdext/source_location.hpp"

#include <fmt/format.h>

#include <concepts>
#include <type_traits>

namespace injectx::stdext {

namespace details::_expects {

template<typename From>
concept convertible_to_bool = std::is_convertible_v<From, bool> || requires {
  static_cast<bool>(std::declval<From>());
};

template<typename Char, typename... Args>
class basic_format_string {
 public:
  template<typename S>
    requires std::convertible_to<const S&, fmt::string_view>
  consteval basic_format_string(
      const S& s,
      stdext::source_location loc = stdext::source_location::current())
      : str_{s},
        loc_(loc) {
  }

  constexpr operator fmt::string_view() const noexcept {
    return str_;
  }

  [[nodiscard]] constexpr auto loc() const noexcept {
    return loc_;
  }

 private:
  fmt::basic_format_string<Char, Args...> str_;
  stdext::source_location loc_;
};

template<typename... Args>
using format_string = basic_format_string<char, fmt::type_identity_t<Args>...>;

[[noreturn]] INJECTX_STDEXT_EXPORT void terminate(
    std::string_view msg, stdext::source_location sl) noexcept;

}  // namespace details::_expects

template<typename Condition, typename... Args>
  requires details::_expects::convertible_to_bool<Condition>
constexpr void expects(
    const Condition& condition,
    [[maybe_unused]] details::_expects::format_string<char, Args...> fmt,
    [[maybe_unused]] Args&&... args) {
  if (condition) {
    return;
  }

  if (std::is_constant_evaluated()) {
    throw "condition failed";
  } else {
    details::_expects::terminate(
        fmt::vformat(fmt, fmt::make_format_args(args...)), fmt.loc());
  }
}

template<typename Condition>
constexpr void expects(
    const Condition& condition,
    [[maybe_unused]] stdext::source_location loc =
        stdext::source_location::current()) {
  if (condition) {
    return;
  }

  if (std::is_constant_evaluated()) {
    throw "condition failted: 'expected to be true'";
  } else {
    details::_expects::terminate("expected to be true", loc);
  }
}

}  // namespace injectx::stdext
