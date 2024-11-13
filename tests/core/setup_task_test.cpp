// SPDX-FileCopyrightText: Copyright 2023 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/setup_task.hpp"

#include "injectx/stdext/expected.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <vector>

namespace injectx::core::tests {

template <typename T>
concept MakeSetupTaskWith = IsSetupTask<SetupTask<T>>;

TEST_CASE("concept") {
  STATIC_REQUIRE(MakeSetupTaskWith<void>);

  struct Provides {};
  STATIC_REQUIRE(MakeSetupTaskWith<Provides>);

  STATIC_REQUIRE(MakeSetupTaskWith<int> == false);

  auto setup = []() -> SetupTask<void> {
    co_yield {};
    // co_return;
  };

  auto task = setup();
  const auto provides = task.init();
  REQUIRE(provides.has_value());
}

TEST_CASE("init-missing-co-yield") {
  struct Provides {};

  auto setup = []() -> SetupTask<Provides> {
    co_return;
  };

  auto task = setup();
  const auto provides = task.init();
  REQUIRE(!provides.has_value());
  REQUIRE(provides.error() ==
          std::string_view{"SetupTask is missing co_yield"});
}

TEST_CASE("init-co-yield-provides") {
  struct Provides {
    int foo;
  };

  auto setup = []() -> SetupTask<Provides> {
    co_yield {.foo = 10};
    co_return;
  };

  auto task = setup();
  const auto provides = task.init();
  REQUIRE(provides.has_value());
  REQUIRE(provides->foo == 10);
}

TEST_CASE("init-co-yield-error") {
  struct Provides {
    int foo;
  };

  auto setup = []() -> SetupTask<Provides> {
    co_yield stdext::unexpected{"something is wrong"};
    co_return;
  };

  auto task = setup();
  const auto provides = task.init();
  REQUIRE(!provides.has_value());
  REQUIRE(provides.error() == std::string_view{"something is wrong"});
}

TEST_CASE("init-twice-co-yield-provides") {
  struct Provides {};

  auto setup = []() -> SetupTask<Provides> {
    co_yield {};
    co_return;
  };

  auto task = setup();
  REQUIRE(task.init().has_value());

  const auto provides = task.init();
  REQUIRE(!provides.has_value());
  REQUIRE(provides.error() ==
          std::string_view{"SetupTask has been already inialized"});
}

TEST_CASE("init-co-yield-no-teardown-in-dtor") {
  std::vector<int> steps;

  {
    struct Provides {};
    auto setup = [&steps]() -> SetupTask<Provides> {
      steps.push_back(1);
      co_yield {};
      steps.push_back(2);
      co_return;
    };

    auto task = setup();
    REQUIRE(steps.empty());

    const auto provides = task.init();
    REQUIRE(provides.has_value());
    REQUIRE(steps == std::vector{1});
  }

  REQUIRE(steps == std::vector{1});
}

TEST_CASE("init-and-teardown-co-yield-provides") {
  std::vector<int> steps;

  struct Provides {};
  auto setup = [&steps]() -> SetupTask<Provides> {
    steps.push_back(1);
    co_yield {};
    steps.push_back(2);
    co_yield {};
    steps.push_back(3);
    co_return;
  };

  auto task = setup();
  REQUIRE(task.init().has_value());

  const auto result = task.teardown();
  REQUIRE(!result.has_value());
  REQUIRE(result.error() ==
          std::string_view{"SetupTask co_yield twice with Provides{}"});
  REQUIRE(steps == std::vector{1, 2});
}

TEST_CASE("init-and-teardown-co-yield-error") {
  std::vector<int> steps;

  struct Provides {};
  auto setup = [&steps]() -> SetupTask<Provides> {
    steps.push_back(1);
    co_yield {};
    steps.push_back(2);
    co_yield stdext::unexpected{"something is really wrong"};
    steps.push_back(3);
    co_return;
  };

  auto task = setup();
  REQUIRE(task.init().has_value());

  const auto result = task.teardown();
  REQUIRE(!result.has_value());
  REQUIRE(result.error() == std::string_view{"something is really wrong"});
  REQUIRE(steps == std::vector{1, 2});
}

}  // namespace injectx::core::tests
