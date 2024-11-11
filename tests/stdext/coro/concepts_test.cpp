// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/coro/concepts.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::stdext::coro::tests {

TEST_CASE("awaiter-std-types") {
  STATIC_REQUIRE(awaiter<stdext::suspend_always>);
  STATIC_REQUIRE(awaiter<stdext::suspend_never>);
}

TEST_CASE("non-awaiter") {
  STATIC_REQUIRE_FALSE(awaiter<int>);
}

TEST_CASE("awaiter-bool-await-suspend-no-args") {
  struct Some {
    bool await_ready() {
      return true;
    }

    bool await_suspend() {
      return false;
    }

    void await_resume() {
    }
  };

  STATIC_REQUIRE(awaiter<Some>);
}

TEST_CASE("awaiter-bool-await-suspend-arg-type-erased-handle") {
  struct Some {
    bool await_ready() {
      return true;
    }

    void await_suspend(stdext::coroutine_handle<>) {
    }

    void await_resume() {
    }
  };

  STATIC_REQUIRE(awaiter<Some>);
}

TEST_CASE("non-awaitable") {
  STATIC_REQUIRE_FALSE(awaitable<int>);
}

TEST_CASE("awaitable-co-await-as-member-function") {
  struct Some {
    auto operator co_await() noexcept {
      return stdext::suspend_always{};
    }
  };

  STATIC_REQUIRE(awaitable<Some>);
}

namespace free_co_await {

struct Some {};

auto operator co_await(Some) noexcept {
  return stdext::suspend_always{};
}

}  // namespace free_co_await

TEST_CASE("awaitable-co-await-as-free-function") {
  STATIC_REQUIRE(awaitable<free_co_await::Some>);
}

}  // namespace injectx::stdext::coro::tests
