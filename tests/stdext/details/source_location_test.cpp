// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/details/source_location.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

namespace injectx::stdext::details::tests {

using source_location = _source_location::source_location;

[[nodiscard]] constexpr auto foo() noexcept {
  return source_location::current();
}

TEST_CASE("current-inside-function") {
  constexpr auto sl = foo();
  STATIC_REQUIRE(sl.line() == 15);
  STATIC_REQUIRE(
      std::string_view{sl.function_name()} == std::string_view{"foo"});
  STATIC_REQUIRE(
      std::string_view{sl.file_name()}.ends_with("source_location_test.cpp"));
}

[[nodiscard]] constexpr auto foo2(
    source_location sl = source_location::current()) noexcept {
  return sl;
}

TEST_CASE("current-as-argument") {
  constexpr auto sl = foo2();
  STATIC_REQUIRE(sl.line() == 33);
  STATIC_REQUIRE(
      std::string_view{sl.function_name()} == std::string_view{__func__});
  STATIC_REQUIRE(
      std::string_view{sl.file_name()}.ends_with("source_location_test.cpp"));
}

}  // namespace injectx::stdext::details::tests
