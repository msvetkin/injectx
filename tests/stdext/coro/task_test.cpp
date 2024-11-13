// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/coro/task.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <concepts>
#include <functional>
#include <type_traits>

namespace injectx::stdext::coro::tests {

TEMPLATE_TEST_CASE("type-traits", "", void, int) {
  using Task = task<TestType>;

  STATIC_REQUIRE(std::same_as<typename Task::value_type, TestType>);
  STATIC_REQUIRE(std::default_initializable<Task> == false);
  STATIC_REQUIRE(std::copy_constructible<Task> == false);
  STATIC_REQUIRE(std::move_constructible<Task>);
  STATIC_REQUIRE(
      std::constructible_from<Task, typename stdext::coroutine_handle<
                                        typename Task::promise_type>>);
}

TEMPLATE_TEST_CASE("make-task", "", void, int) {
  auto t = std::invoke([]() -> task<TestType> {
    if constexpr (std::is_void_v<TestType>) {
      co_return;
    } else {
      co_return {};
    }
  });

  REQUIRE(t.is_ready().has_value());
  REQUIRE(t.is_ready().value() == false);
}

TEMPLATE_TEST_CASE("execution-order", "", void, int) {
  bool executed = false;

  auto t = std::invoke(
      [](bool& executed) -> task<TestType> {
        executed = true;

        if constexpr (std::is_void_v<TestType>) {
          co_return;
        } else {
          co_return {};
        }
      },
      executed);

  REQUIRE(executed == false);
  REQUIRE(t.is_ready().has_value());
  REQUIRE(t.is_ready().value() == false);
  REQUIRE(t.resume().has_value());

  REQUIRE(executed == true);
  REQUIRE(t.is_ready().has_value());
  REQUIRE(t.is_ready().value() == true);

  const auto r2 = t.resume();
  REQUIRE(r2.has_value() == false);
  REQUIRE(r2.error() ==
          std::string_view{"coroutine is in the final suspend point"});

  if constexpr (std::is_void_v<TestType>) {
    STATIC_REQUIRE(std::is_void_v<decltype(t.promise().result())>);
    t.promise().result();
  } else {
    STATIC_REQUIRE(std::same_as<decltype(t.promise().result()), TestType>);
    REQUIRE(t.promise().result() == TestType{});
  }
}

TEST_CASE("ctor-move") {
  auto t = std::invoke([]() -> task<void> {
    co_return;
  });

  REQUIRE(t.is_ready().has_value());
  REQUIRE(t.is_ready().value() == false);

  auto t2 = std::move(t);
  REQUIRE(t.is_ready().has_value() == false);
  REQUIRE(t.is_ready().error() == std::string_view{"coroutine handle is null"});
  REQUIRE(t2.is_ready().has_value());
  REQUIRE(t2.is_ready().value() == false);

  t = std::move(t2);

  REQUIRE(t2.is_ready().has_value() == false);
  REQUIRE(t2.is_ready().error() ==
          std::string_view{"coroutine handle is null"});
  REQUIRE(t.is_ready().has_value());
  REQUIRE(t.is_ready().value() == false);
}

TEST_CASE("continuation") {
  bool executedFoo = false;
  bool executedBoo = false;

  const auto foo = [&executedFoo]() -> task<int> {
    executedFoo = true;
    co_return 10;
  };

  const auto boo = [&foo, &executedBoo]() -> task<int> {
    executedBoo = true;
    co_return co_await foo();
  };

  auto t = boo();
  REQUIRE(t.is_ready().value() == false);

  REQUIRE(t.resume().has_value());
  REQUIRE(t.is_ready().value() == true);
  REQUIRE(executedBoo == true);
  REQUIRE(executedFoo == true);
}

TEST_CASE("loop-stack-overflow") {
  auto loop = [](const int count) -> task<int> {
    auto one_iteration = []() -> task<void> {
      co_return;
    };

    int i = 0;
    for (; i < count; ++i) {
      co_await one_iteration();
    }

    co_return i;
  };

  const int count = 1'000'000;

  auto runTask = std::invoke(
      [](auto t) -> task<int> {
        co_return co_await t;
      },
      loop(count));

  REQUIRE(runTask.resume().has_value());
  const auto ready = runTask.is_ready();
  REQUIRE(ready.has_value());
  REQUIRE(ready.value());
  REQUIRE(runTask.promise().result() == count);
}

}  // namespace injectx::stdext::coro::tests
