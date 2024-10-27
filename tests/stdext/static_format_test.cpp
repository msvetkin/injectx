// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/static_format.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>

namespace injectx::stdext::tests {

TEST_CASE("from-const-char") {
  constexpr auto msg = static_format<"hello {}", 1>();
  STATIC_REQUIRE(msg == std::string_view{"hello 1"});
}

TEST_CASE("static-string") {
  constexpr auto msg = static_format<"hello {}", static_string{"world"}>();
  STATIC_REQUIRE(msg == std::string_view{"hello world"});
}

}  // namespace injectx::stdext::tests
