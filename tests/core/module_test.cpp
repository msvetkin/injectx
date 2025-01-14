// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/module.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::core::tests {

namespace modules::first {

struct Requires {};

struct Provides {
  std::shared_ptr<int> input;
};

SetupTask<Provides> setup(Requires) {
  auto input = std::make_shared<int>(50);
  co_yield Provides{.input = input};
  co_return;
};

}  // namespace modules::first

TEST_CASE("sanity") {
  constexpr auto module = makeModule<modules::first::setup>();
  STATIC_REQUIRE(module.has_value());
  DependencyContainer dependencyContainer;
  auto t = module->setup(dependencyContainer);

  REQUIRE(t.init().has_value());

  const auto provides = dependencyContainer.resolve<modules::first::Provides>();
  REQUIRE(provides.has_value());
  REQUIRE(provides->input);
  REQUIRE(*provides->input == 50);

  REQUIRE(t.teardown().has_value());

  REQUIRE(provides->input);
  REQUIRE(*provides->input == 50);
}

namespace modules::second {

struct Requires {};

struct Provides {
  std::shared_ptr<int> input;
};

SetupTask<Provides> setup(Requires) {
  auto input = std::make_shared<int>(50);
  co_yield Provides{.input = input};

  *input = 100;
};

}  // namespace modules::second

TEST_CASE("sanity-2") {
  DependencyContainer dependencyContainer;
  std::shared_ptr<int> input;
  {
    constexpr auto module = makeModule<modules::second::setup>();
    STATIC_REQUIRE(module.has_value());
    auto t = module->setup(dependencyContainer);
    REQUIRE(t.init().has_value());

    const auto provides =
        dependencyContainer.resolve<modules::second::Provides>();
    REQUIRE(provides.has_value());
    input = provides->input;

    REQUIRE(*input == 50);

    REQUIRE(t.teardown().has_value());
  }

  REQUIRE(*input == 100);
}

namespace modules::third {

struct Requires {
  int value;
};

struct Provides {
  int foo;
};

SetupTask<Provides> setup(Requires) {
  co_yield {.foo = 10};
};

}  // namespace modules::third

TEST_CASE("missing-dependencies") {
  DependencyContainer dependencyContainer;
  constexpr auto module = makeModule<modules::third::setup>();
  STATIC_REQUIRE(module.has_value());
  auto t = module->setup(dependencyContainer);
  const auto res = t.init();

  REQUIRE(res.has_value() == false);
  REQUIRE(
      res.error()
      == std::string_view{"Dependency 'int value' has not been provided"});
}

TEST_CASE("provide-twice") {
  DependencyContainer dependencyContainer;
  constexpr auto module = makeModule<modules::second::setup>();
  STATIC_REQUIRE(module.has_value());

  auto setupTask1 = module->setup(dependencyContainer);
  REQUIRE(setupTask1.init().has_value());

  auto setupTask2 = module->setup(dependencyContainer);
  const auto res = setupTask2.init();

  REQUIRE(res.has_value() == false);
  REQUIRE(
      res.error()
      == std::string_view{
          "Dependency 'std::shared_ptr<int> input' has been already provided"});
}

namespace modules::fourth {

struct Requires {
  int value;
};

struct Provides {
  int foo;
};

SetupTask<Provides> setup(Requires deps) {
  if (deps.value == 10) {
    co_yield stdext::unexpected{"something went wrong"};
  }

  co_yield {.foo = deps.value};
};

}  // namespace modules::fourth

TEST_CASE("error-during-init") {
  const modules::fourth::Requires deps{.value = 10};
  DependencyContainer dependencyContainer;
  REQUIRE(dependencyContainer.provide(deps).has_value());

  constexpr auto module = makeModule<modules::fourth::setup>();
  STATIC_REQUIRE(module.has_value());

  auto t = module->setup(dependencyContainer);
  const auto result = t.init();
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error() == std::string_view{"something went wrong"});

  const auto p = dependencyContainer.resolve<modules::fourth::Provides>();
  REQUIRE(p.has_value() == false);
}

namespace modules::fifth {

std::vector<int> gSteps;

struct Requires {
  int value;
};

struct Provides {
  int foo;
};

SetupTask<Provides> setup(Requires deps) {
  gSteps.push_back(1);
  co_yield {.foo = deps.value};
  gSteps.push_back(2);
  co_yield stdext::unexpected{"something went wrong"};
  gSteps.push_back(3);
};

}  // namespace modules::fifth

TEST_CASE("error-during-teardown") {
  const modules::fifth::Requires deps{.value = 10};
  DependencyContainer dependencyContainer;
  REQUIRE(dependencyContainer.provide(deps).has_value());

  constexpr auto module = makeModule<modules::fifth::setup>();
  STATIC_REQUIRE(module.has_value());
  REQUIRE(modules::fifth::gSteps.empty());

  auto t = module->setup(dependencyContainer);
  REQUIRE(t.init().has_value());
  REQUIRE(dependencyContainer.resolve<modules::fifth::Provides>().has_value());
  REQUIRE(modules::fifth::gSteps == std::vector{1});

  const auto result = t.teardown();
  REQUIRE(result.has_value() == false);
  REQUIRE(result.error() == std::string_view{"something went wrong"});
  REQUIRE(modules::fifth::gSteps == std::vector{1, 2});
}

}  // namespace injectx::core::tests
