// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/bundle.hpp"

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
  STATIC_REQUIRE(bundle->size() == 1);
}

namespace modules::second {

// class Some {};

struct Requires {
  int value;
  // Some some;
};

struct Provides {
  float output;
  std::string_view name;
};

SetupTask<Provides> setup(Requires) {
  co_yield {.output = 1.0, .name = "test"};
}

}  // namespace modules::second

TEST_CASE("constexpr-missing-dependencies") {
  constexpr auto bundle = makeBundle<modules::second::setup>();

  STATIC_REQUIRE(bundle.has_value() == false);
  STATIC_REQUIRE(
      bundle.error()
      == std::string_view{
          "Component 'second' could not resolve dependency: int value"});
}

namespace modules::third {

struct Provides {
  int value;
};

SetupTask<Provides> setup() {
  co_yield {.value = 1};
}

}  // namespace modules::third

TEST_CASE("constexpr-order") {
  constexpr auto bundle = makeBundle<
      modules::second::setup, modules::first::setup, modules::third::setup>();

  STATIC_REQUIRE(bundle.has_value());
  STATIC_REQUIRE(bundle->at(0).name() == std::string_view{"first"});
  STATIC_REQUIRE(bundle->at(1).name() == std::string_view{"third"});
  STATIC_REQUIRE(bundle->at(2).name() == std::string_view{"second"});
}

namespace modules::forth {

struct Provides {
  int value;
};

SetupTask<Provides> setup() {
  co_yield {.value = 1};
}

}  // namespace modules::forth

TEST_CASE("conflict") {
  constexpr auto bundle = makeBundle<
      modules::second::setup, modules::first::setup, modules::third::setup,
      modules::forth::setup>();

  STATIC_REQUIRE(bundle.has_value() == false);
  REQUIRE(
      bundle.error()
      == std::string_view{
          "Dependency 'value' provided by two modules 'third' and 'forth'"});
}

}  // namespace injectx::core::tests
