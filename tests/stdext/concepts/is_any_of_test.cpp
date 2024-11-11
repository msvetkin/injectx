// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/concepts/is_any_of.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::stdext::tests {

TEST_CASE("scalar-types") {
  STATIC_REQUIRE(stdext::is_any_of<int, float, double, int>);
  STATIC_REQUIRE_FALSE(stdext::is_any_of<int, float, double>);
}

}  // namespace injectx::stdext::tests
