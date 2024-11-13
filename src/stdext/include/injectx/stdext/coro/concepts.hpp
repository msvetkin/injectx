// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/concepts/is_any_of.hpp"
#include "injectx/stdext/coroutine.hpp"

namespace injectx::stdext::coro {

template<typename T>
concept await_suspend_return_type =
    stdext::is_any_of<T, void, bool>
    || std::convertible_to<T, stdext::coroutine_handle<>>;

template<typename T>
concept await_suspend_no_arg = requires(T t) {
  { t.await_suspend() } -> await_suspend_return_type;
};

template<typename T>
concept await_suspend_with_arg = requires(T t, stdext::coroutine_handle<> c) {
  { t.await_suspend(c) } -> await_suspend_return_type;
};

template<typename T>
concept awaiter = requires(T t) {
  { t.await_ready() } -> std::same_as<bool>;
  requires await_suspend_no_arg<T> || await_suspend_with_arg<T>;
  { t.await_resume() };
};

template<typename T>
concept has_member_operator_co_await = requires(T t) {
  { t.operator co_await() } -> awaiter;
};

template<typename T>
concept has_free_operator_co_await = requires(T t) {
  { operator co_await(t) } -> awaiter;
};

template<typename T>
concept awaitable =
    has_member_operator_co_await<T> || has_free_operator_co_await<T>;

}  // namespace injectx::stdext::coro
