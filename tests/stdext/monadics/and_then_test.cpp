// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/monadics/and_then.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::stdext::monadics::tests {

struct Some {
  int value;
};

template<typename F>
constexpr auto and_then_as(const Some &s, F f) {
  return f(s);
}

TEST_CASE("concept") {
  using F = decltype([](auto) {
    return 1;
  });
  STATIC_REQUIRE(and_then.invocable<Some, F>);
  STATIC_REQUIRE(and_then.invocable<int &, F> == false);
}

TEST_CASE("pipe-operator") {
  constexpr Some s0{.value = 10};
  constexpr auto s1 = s0 | and_then([](auto s) {
                        return s.value + 10;
                      });

  STATIC_REQUIRE(s0.value == 10);
  STATIC_REQUIRE(s1 == 20);
}

}  // namespace injectx::stdext::monadics::tests
