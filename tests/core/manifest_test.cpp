// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/manifest.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::core::tests {

namespace modules::first {

struct Requires {};

struct Provides {};

SetupTask<Provides> setup(Requires) {
  co_yield {};
}

}  // namespace modules::first

TEST_CASE("provides-and-requires-empty") {
  constexpr auto manifest = makeManifest(modules::first::setup);
  STATIC_REQUIRE(manifest.has_value());
  STATIC_REQUIRE(manifest->name() == "first");
  STATIC_REQUIRE(manifest->dependencies().size() == 0);
  STATIC_REQUIRE(manifest->provides().size() == 0);
}

namespace modules::second {

class Some {};

struct Requires {
  int value;
  Some some;
};

struct Provides {
  float output;
  std::string_view name;
};

SetupTask<Provides> setup(Requires) {
  co_yield {.output = 1.0, .name = "test"};
}

}  // namespace modules::second

TEST_CASE("provides-and-requires") {
  constexpr auto manifest = makeManifest(modules::second::setup);
  STATIC_REQUIRE(manifest.has_value() == true);
  STATIC_REQUIRE(manifest->name() == "second");

  constexpr std::array dependencies = {
      DependencyInfo{.type = "int", .name = "value"},
      DependencyInfo{
          .type = "injectx::core::tests::modules::second::Some",
          .name = "some"}};

  STATIC_REQUIRE(manifest->dependencies() == gsl::span{dependencies});

  constexpr std::array provides = {
      DependencyInfo{.type = "float", .name = "output"},
      DependencyInfo{
          .type = stdext::type_name<std::string_view>(), .name = "name"}};

  STATIC_REQUIRE(manifest->provides() == gsl::span{provides});
}

namespace modules::third {

struct Provides {};

SetupTask<Provides> setup() {
  co_yield {};
}

}  // namespace modules::third

TEST_CASE("only-provides") {
  constexpr auto manifest = makeManifest(modules::third::setup);
  STATIC_REQUIRE(manifest.has_value());
  STATIC_REQUIRE(manifest->name() == "third");
  STATIC_REQUIRE(manifest->dependencies().size() == 0);
  STATIC_REQUIRE(manifest->provides().size() == 0);
}

namespace modules::forth {

struct Requires {};

SetupTask<void> setup(Requires) {
  co_yield {};
}

}  // namespace modules::forth

TEST_CASE("only-requires") {
  constexpr auto manifest = makeManifest(modules::forth::setup);
  STATIC_REQUIRE(manifest.has_value());
  REQUIRE(manifest->name() == "forth");
  STATIC_REQUIRE(manifest->dependencies().size() == 0);
  STATIC_REQUIRE(manifest->provides().size() == 0);
}

namespace boo {

struct Requires {
  int foo;
};

struct Provides {
  int foo;
};

SetupTask<Provides> setup(Requires) {
  co_yield {};
}

}  // namespace boo

TEST_CASE("invalid-namespace") {
  constexpr auto manifest = makeManifest(boo::setup);
  STATIC_REQUIRE(manifest.has_value() == false);
  STATIC_REQUIRE(manifest.error() == std::string_view{"invalid namespace"});
}

namespace modules::boo {

struct Requires {
  int foo;
};

struct Provides {
  int foo;
};

SetupTask<Provides> setup(Requires) {
  co_yield {};
}

}  // namespace modules::boo

TEST_CASE("error-depends-on-itself") {
  constexpr auto manifest = makeManifest(modules::boo::setup);
  STATIC_REQUIRE(manifest.has_value() == false);
  STATIC_REQUIRE(
      manifest.error()
      == std::string_view{"Component 'boo' provides and depends on 'foo'"});
}

}  // namespace injectx::core::tests
