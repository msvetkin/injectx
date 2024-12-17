// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/monadics/pipe.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <tuple>

namespace injectx::stdext::monadics::tests {

template<typename Callback>
struct some_action {
  Callback callback;

  template<typename T>
  [[nodiscard]] friend constexpr auto operator|(T &&t, some_action &&s) noexcept
    requires std::invocable<decltype(s.callback), decltype(t)>
  {
    return std::invoke(std::forward<Callback>(s.callback), std::forward<T>(t));
  }
};

inline constexpr pipe_fn<some_action> some{};

TEST_CASE("concept") {
  using F = decltype([](auto) {
    return 1;
  });
  STATIC_REQUIRE(some.invocable<int &, F>);
}

TEST_CASE("operator") {
  constexpr auto r = std::invoke([] {
    int v0 = 10;
    auto v1 = v0 | some([](int &v) {
                auto s = v;
                v = 15;
                return s + 10;
              });
    return std::tuple{v0, v1};
  });

  STATIC_REQUIRE(std::get<0>(r) == 15);
  STATIC_REQUIRE(std::get<1>(r) == 20);
}

}  // namespace injectx::stdext::monadics::tests
