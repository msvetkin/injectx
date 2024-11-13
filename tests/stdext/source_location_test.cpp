// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/source_location.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

namespace injectx::stdext::tests {

TEST_CASE("sanity-check") {
  constexpr auto sl = source_location::current();
  STATIC_REQUIRE(sl.line() == 13);
  STATIC_REQUIRE(
      std::string_view{sl.file_name()}.ends_with("source_location_test.cpp"));
}

}  // namespace injectx::stdext::tests
