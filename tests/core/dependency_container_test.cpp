// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/dependency_container.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

namespace injectx::core::tests {

namespace first {

struct Provides {
  int i;
  bool b;
  float f;
};

struct Requires {
  int i{};
  bool b{};
  float f{};
};

}  // namespace first

TEST_CASE("provide-pod-types") {
  using namespace first;

  DependencyContainer dependencies;

  Provides provides{.i = 5, .b = false, .f = 55.0};
  REQUIRE(dependencies.provide(provides).has_value());

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value());
  REQUIRE(resolved->i == 5);
  REQUIRE(resolved->i == provides.i);
  REQUIRE(std::addressof(resolved->i) != std::addressof(provides.i));
  REQUIRE(resolved->b == false);
  REQUIRE(resolved->b == provides.b);
  REQUIRE(std::addressof(resolved->b) != std::addressof(provides.b));
  REQUIRE(resolved->f == 55.0);
  REQUIRE(resolved->f == provides.f);
  REQUIRE(std::addressof(resolved->f) != std::addressof(provides.f));
}

namespace second {

struct Provides {
  std::function<int()> value;
};

struct Requires {
  int value{555};
};

}  // namespace second

TEST_CASE("provide-callable") {
  using namespace second;

  DependencyContainer dependencies;

  Provides provides{.value = [] {
    static int value = 0;
    return value++;
  }};
  REQUIRE(dependencies.provide(provides).has_value());

  const auto resolved1 = dependencies.resolve<Requires>();
  REQUIRE(resolved1.has_value());
  REQUIRE(resolved1->value == 0);

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value());
  REQUIRE(resolved->value == 1);
}

namespace third {

struct Provides {
  std::shared_ptr<int> value;
};

struct Requires {
  std::shared_ptr<int> value;
};

}  // namespace third

TEST_CASE("provide-shared-pointer") {
  DependencyContainer dependencies;

  Provides provides{.value = std::make_shared<int>(100)};
  REQUIRE(dependencies.provide(provides).has_value());

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value());
  REQUIRE(*resolved->value == 100);
  REQUIRE(resolved->value == provides.value);
  REQUIRE(provides.value.use_count() == 3);
}

namespace forth {

struct Provides {
  int value;
  int boo;
};

}  // namespace forth

TEST_CASE("provide-same-type-twice") {
  using namespace forth;

  DependencyContainer dependencies;

  Provides provides1{.value = 10, .boo = 12};
  REQUIRE(dependencies.provide(provides1).has_value());

  Provides provides2{.value = 55, .boo = 66};
  const auto provided = dependencies.provide(provides2);
  REQUIRE(provided.has_value() == false);
  REQUIRE(
      provided.error()
      == std::string_view{"Dependency 'int value' has been already provided"});

  const auto resolved = dependencies.resolve<Provides>();
  REQUIRE(resolved.has_value());
  REQUIRE(resolved->value == 10);
  REQUIRE(resolved->value == provides1.value);
  REQUIRE(resolved->boo == 12);
  REQUIRE(resolved->boo == provides1.boo);
}

namespace fifth {

struct Provides {
  int i;
};

struct Requires {
  int i;
  bool b;
  float f;
};

}  // namespace fifth

TEST_CASE("provide-half-of-requires") {
  using namespace fifth;

  DependencyContainer dependencies;

  Provides provides{.i = 10};
  REQUIRE(dependencies.provide(provides).has_value());

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value() == false);
  REQUIRE(
      resolved.error()
      == std::string_view{"Dependencies 'bool b' has not been provided, "
                          "'float f' has not been provided"});
}

namespace six {

struct Provides {
  float foo;
  int boo;
};

struct Requires {
  int foo;
  float boo;
};

}  // namespace six

TEST_CASE("provide-same-names-different-types-then-requires") {
  using namespace six;

  DependencyContainer dependencies;

  Provides provides{.foo = 1.0, .boo = 2};
  REQUIRE(dependencies.provide(provides).has_value());

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value() == false);
  REQUIRE(
      resolved.error()
      == std::string_view{"Dependencies 'int foo' has different type, 'float "
                          "boo' has different type"});
}

namespace seven {

struct Provides1 {
  int i;
  bool b;
};

struct Provides2 {
  float f;
  int i;
};

struct Requires {
  float f;
};

}  // namespace seven

TEST_CASE("provide-conflictig-types") {
  using namespace seven;

  DependencyContainer dependencies;

  Provides1 provides1{.i = 32, .b = true};
  REQUIRE(dependencies.provide(provides1).has_value());

  Provides2 provides2{.f = 91.5, .i = 9};
  const auto provided2 = dependencies.provide(provides2);
  REQUIRE(provided2.has_value() == false);
  REQUIRE(
      provided2.error()
      == std::string_view{"Dependency 'int i' has been already provided"});

  const auto resolved = dependencies.resolve<Requires>();
  REQUIRE(resolved.has_value() == false);
  REQUIRE(
      resolved.error()
      == std::string_view{"Dependency 'float f' has not been provided"});
}

}  // namespace injectx::core::tests
