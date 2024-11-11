// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/coroutine.hpp"

#include <catch2/catch_test_macros.hpp>

namespace injectx::stdext::tests {

TEST_CASE("types-available") {
  struct task {
    struct promise_type {
      task get_return_object() {
        return {coroutine_handle<promise_type>::from_promise(*this)};
      }

      suspend_never initial_suspend() {
        return {};
      }

      suspend_always final_suspend() noexcept {
        return {};
      }

      void return_void() {
      }

      void unhandled_exception() {
      }
    };

    ~task() {
      if (handle) {
        handle.destroy();
      }
    }

    coroutine_handle<promise_type> handle;
  };

  auto t = []() -> task {
    co_return;
  }();

  REQUIRE(t.handle.done());

  coroutine_handle<> handle = noop_coroutine();
  REQUIRE_FALSE(handle.done());
}

}  // namespace injectx::stdext::tests
