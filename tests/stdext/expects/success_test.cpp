// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expects.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>

namespace injectx::stdext::tests {

constexpr bool foo() {
  const bool value = true;
  expects(value, "bool should pass");
  expects(value);

  auto *valuePointer = &value;
  expects(valuePointer, "bool pointer should pass");

  struct Some {
    constexpr explicit operator bool() const noexcept {
      return true;
    }
  };

  constexpr Some some;
  expects(some, "type with operator bool should pass");

  return true;
}

TEST_CASE("constexpr") {
  constexpr auto result = std::invoke([]() {
    return foo();
  });

  STATIC_REQUIRE(result);
}

TEST_CASE("runtime") {
  REQUIRE(foo());
}

}  // namespace injectx::stdext::tests
