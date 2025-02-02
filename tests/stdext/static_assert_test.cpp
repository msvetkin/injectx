// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

// #include "injectx/stdext/static_assert.hpp"
// #include "injectx/stdext/expected.hpp"
#include "injectx/stdext/static_format.hpp"

#include <fmt/compile.h>
#include <fmt/format.h>

#include <catch2/catch_test_macros.hpp>

// namespace experiment {

// template<std::size_t N>
// struct static_string {
// consteval explicit(false) static_string() noexcept {
// }

// consteval explicit(false) static_string(const char (&str)[N]) noexcept {
// for (auto i = std::size_t{}; i < N; i++) {
// data_[i] = str[i];
// }
// }

// friend constexpr std::strong_ordering operator<=>(
// const static_string &, const static_string &) = default;

// std::array<char, N> data_{};
// };

// template<static_string fmt, auto... args>
// [[nodiscard]] consteval auto static_format() noexcept {
// constexpr auto fmtCompiled = FMT_COMPILE(fmt.data_.data());
// constexpr auto storageSize = fmt::formatted_size(fmtCompiled, args...);
// char msg[storageSize + 1]{};
// fmt::format_to(msg, fmtCompiled, args...);
// return static_string{msg};
// }

// } // namespace experiment

// template<std::size_t N>
// struct fmt::formatter<experiment::static_string<N>> {
// constexpr auto parse(fmt::format_parse_context &ctx) {
// return ctx.begin();
// }

// template<typename FormatContext>
// constexpr auto format(
// const experiment::static_string<N> &s, FormatContext &ctx) const {
// return fmt::format_to(ctx.out(), FMT_COMPILE("{}"), s.data());
// }
// };

namespace experiment {

#if (defined(__GNUC__) && !defined(__clang__) && __GNUC__ <= 13) \
    || defined(_MSC_VER)

template<bool C, std::array msg>
struct panic {
  static consteval bool emit() {
    return true;
  }
};

template<std::array msg>
struct panic<false, msg> {
  static consteval bool emit() = delete;
};

template<bool C, std::array msg>
inline constexpr bool verify = panic<C, msg>::emit();

#else

template<bool C, std::array msg>
concept panic = C;

template<bool C, std::array msg>
concept verify = panic<C, msg>;

#endif

}  // namespace experiment

#define EXP_STATIC_ASSERT(cond, fmt, ...)                        \
  [&]<bool Condition>() consteval {                              \
    if constexpr (!Condition) {                                  \
      constexpr auto msg =                                       \
          injectx::stdext::static_format2<fmt, __VA_ARGS__>();   \
      static_assert(experiment::verify<Condition, msg.storage>); \
    }                                                            \
  }.template operator()<cond>()

TEST_CASE("success") {
  EXP_STATIC_ASSERT(2 == 1, "That is wrong {}", 1);
}
