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
  constexpr static_string(const char (&str)[N]) noexcept
      : size_{N - 1} {
    std::copy(std::begin(str), std::end(str), data_.begin());
  }

  constexpr static_string(std::string_view str) noexcept {
    for (std::size_t i = 0; i < str.size(); ++i) {
      data_[i] = str[i];
    }
    size_ = str.size();
  }

  friend constexpr std::strong_ordering operator<=>(
      const static_string &, const static_string &) = default;

  [[nodiscard]] constexpr const char *data() const noexcept {
    return data_.data();
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr char operator[](std::size_t idx) const noexcept {
    return data_[idx];
  }

  std::array<char, N + 1> data_{};
  std::size_t size_{};
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

#define STDEXT_AS_STATIC_STRING(s)  \
  stdext::static_string<s.size()> { \
    s                               \
  }
