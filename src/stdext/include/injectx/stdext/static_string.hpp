// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <fmt/compile.h>
#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <string_view>

namespace injectx::stdext {

template<std::size_t N>
struct static_string {
  static inline constexpr auto Capacity = N;
  static inline constexpr auto Size = N - 1;
  static inline constexpr bool Empty = N == 1;

  consteval explicit(false) static_string(const char (&str)[N]) noexcept {
    for (auto i = std::size_t{}; i < N; ++i) {
      storage[i] = str[i];
    }
    // std::copy(std::begin(str), std::end(str), storage.begin());
  }

  consteval explicit(true) static_string(const char *str, std::size_t size) {
    for (auto i = std::size_t{}; i < size; ++i) {
      storage[i] = str[i];
    }
  }

  consteval explicit(true) static_string(std::string_view str) noexcept
      : static_string(str.data(), str.size()) {
  }

  friend constexpr std::strong_ordering operator<=>(
      const static_string &, const static_string &) = default;

  [[nodiscard]] constexpr const char *data() const noexcept {
    return storage.data();
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return Size;
  }

  [[nodiscard]] constexpr char operator[](std::size_t idx) const noexcept {
    return storage[idx];
  }

  constexpr explicit(true) operator std::string_view() const {
    return std::string_view{storage.data(), size()};
  }

  std::array<char, N> storage{};
};

template<std::size_t N>
static_string(const char (&str)[N]) -> static_string<N>;

}  // namespace injectx::stdext

template<std::size_t N>
struct fmt::formatter<injectx::stdext::static_string<N>> {
  constexpr auto parse(fmt::format_parse_context &ctx) {
    return ctx.begin();
  }

  template<typename FormatContext>
  constexpr auto format(
      const injectx::stdext::static_string<N> &s, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), FMT_COMPILE("{}"), s.data());
  }
};

#define STDEXT_AS_STATIC_STRING(s)      \
  stdext::static_string<s.size() + 1> { \
    s                                   \
  }
