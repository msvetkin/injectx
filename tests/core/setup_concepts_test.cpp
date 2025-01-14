// SPDX-FileCopyrightText: Copyright 2023 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/setup_concepts.hpp"

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

namespace injectx::core::tests {

TEST_CASE("lambda-is-not-allowed") {
  struct Provides {};

  auto lambda = []() -> SetupTask<Provides> {
    co_yield {};
  };

  STATIC_REQUIRE(IsSetupFunction<decltype(lambda)> == false);
}

void providesIsNotSetupTask() {
}

TEST_CASE("provides-is-not-setup-task") {
  STATIC_REQUIRE_FALSE(IsSetupFunction<decltype(providesIsNotSetupTask)>);
}

struct Provides {};

SetupTask<Provides> requiresIsNotStruct(int) {
  co_yield {};
}

TEST_CASE("provides-and-requires-is-not-struct") {
  STATIC_REQUIRE_FALSE(IsSetupFunction<decltype(requiresIsNotStruct)>);
}

SetupTask<Provides> extraArguments(Provides, int) {
  co_yield {};
}

TEST_CASE("provides-and-requires-with-extra-argument") {
  STATIC_REQUIRE_FALSE(IsSetupFunction<decltype(extraArguments)>);
}

SetupTask<Provides> onlyProvides() {
  co_yield {};
}

TEST_CASE("only-provides") {
  STATIC_REQUIRE(IsSetupFunction<decltype(onlyProvides)>);
}

SetupTask<Provides> providesAndRequires(Provides) {
  co_yield {};
}

TEST_CASE("provides-and-requires") {
  STATIC_REQUIRE(IsSetupFunction<decltype(providesAndRequires)>);
}

SetupTask<void> onlyRequires(Provides) {
  co_yield {};
}

TEST_CASE("only-requires") {
  STATIC_REQUIRE(IsSetupFunction<decltype(onlyRequires)>);
}

SetupTask<void> noProvidesAndRequires() {
  co_yield {};
}

TEST_CASE("no-provides-and-requires") {
  STATIC_REQUIRE_FALSE(IsSetupFunction<decltype(noProvidesAndRequires)>);
}

template<IsSetupFunction auto setup>
inline constexpr auto IsSetupVariable = true;

TEST_CASE("as-concept-with-auto-template-argument") {
  STATIC_REQUIRE(IsSetupVariable<onlyRequires>);
}

}  // namespace injectx::core::tests
