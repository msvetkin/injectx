// SPDX-FileCopyrightText: Copyright 2023 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/setup_traits.hpp"

#include "injectx/core/setup_task.hpp"

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace injectx::core::tests {

struct Provides {};

SetupTask<Provides> onlyProvides() {
  co_yield {};
}

TEST_CASE("only-provides") {
  using Traits = SetupTraits<onlyProvides>;

  STATIC_REQUIRE(std::same_as<typename Traits::Provides, Provides>);
  STATIC_REQUIRE(std::same_as<typename Traits::Requires, std::monostate>);
}

struct Requires {};

SetupTask<Provides> providesAndRequires(Requires) {
  co_yield {};
}

TEST_CASE("provides-and-requires") {
  using Traits = SetupTraits<providesAndRequires>;

  STATIC_REQUIRE(std::same_as<typename Traits::Provides, Provides>);
  STATIC_REQUIRE(std::same_as<typename Traits::Requires, Requires>);
}

SetupTask<void> onlyRequires(Requires) {
  co_yield {};
}

TEST_CASE("only-requires") {
  using Traits = SetupTraits<onlyRequires>;

  STATIC_REQUIRE(std::same_as<typename Traits::Provides, std::monostate>);
  STATIC_REQUIRE(std::same_as<typename Traits::Requires, Requires>);
}

}  // namespace injectx::core::tests
