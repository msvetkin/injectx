// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/launch.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::core::tests {

namespace modules::first {

struct Requires {};

struct Provides {};

SetupTask<Provides> setup(Requires) {
  co_yield {};
}

}  // namespace modules::first

TEST_CASE("constexpr-success") {
  constexpr auto bundle = makeBundle<modules::first::setup>();
  STATIC_REQUIRE(bundle.has_value() == true);

  auto t = launch(bundle.value());
  REQUIRE(t.init().has_value());
  REQUIRE(t.teardown().has_value());
}

}  // namespace injectx::core::tests
