// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/coroutine.hpp"
#include "injectx/stdext/export_macro.hpp"

#include <exception>
#include <utility>
#include <variant>

#ifdef INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER
#include <atomic>
#endif  // INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER

namespace injectx::stdext::coro {

namespace details::_promise {

template <typename T>
class storage {
  static constexpr bool is_ref = std::is_reference_v<T>;

 public:
  using value_type = std::conditional_t<is_ref, std::add_pointer_t<T>, T>;

  template <typename U = T>
  void set(U&& value) noexcept {
    if constexpr (is_ref) {
      result_.template emplace<value_type>(&value);
    } else {
      result_.template emplace<value_type>(std::forward<U>(value));
    }
  }

  void unhandled_exception() noexcept {
    result_ = std::current_exception();
  }

  [[nodiscard]] T result() {
    if (auto exception = std::get_if<std::exception_ptr>(&result_)) {
      std::rethrow_exception(*exception);
    }

    if constexpr (is_ref) {
      return *std::get<value_type>(result_);
    } else {
      return std::get<value_type>(std::move(result_));
    }
  }

 private:
  std::variant<std::monostate, value_type, std::exception_ptr> result_;
};

template <>
class storage<void> {
 public:
  using value_type = void;

  void result() const {
    if (exception_) {
      std::rethrow_exception(exception_);
    }
  }

  void unhandled_exception() noexcept {
    exception_ = std::current_exception();
  }

 private:
  std::exception_ptr exception_;
};

}  // namespace details::_promise

template <typename T>
using promise_storage = details::_promise::storage<T>;

class INJECTX_STDEXT_EXPORT resume_continuation {
 public:
  [[nodiscard]] bool await_ready() const noexcept;

  template <typename Promise>
  auto await_suspend(stdext::coroutine_handle<Promise> coro) const noexcept {
    return coro.promise().resume_awaiting();
  }

  void await_resume() const noexcept;
};

class INJECTX_STDEXT_EXPORT promise_continuation {
 public:
  [[nodiscard]] stdext::suspend_always initial_suspend() const noexcept;
  [[nodiscard]] resume_continuation final_suspend() const noexcept;

// Symmetric transfer does not work in debug build for GCC:
//  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100897
//  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=94794
// The solution was found here:
// https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer#the-coroutines-ts-solution
#ifdef INJECTX_STDEXT_CORO_NO_SYMMETRIC_TRANSFER
 private:
  std::atomic<bool> state_{false};

 public:
  template <typename Promise>
  [[nodiscard]] bool suspend(
      stdext::coroutine_handle<> awaitingHandle,
      stdext::coroutine_handle<Promise> currentHandle) noexcept {
    currentHandle.resume();
    continuation_ = awaitingHandle;
    return !state_.exchange(true, std::memory_order_acq_rel);
  }

  void resume_awaiting() noexcept;

#else  // symmetric transfer
  template <typename Promise>
  [[nodiscard]] stdext::coroutine_handle<> suspend(
      stdext::coroutine_handle<> awaitingHandle,
      stdext::coroutine_handle<Promise> currentHandle) noexcept {
    continuation_ = awaitingHandle;
    return currentHandle;
  }

  [[nodiscard]] stdext::coroutine_handle<> resume_awaiting() noexcept;
#endif

 private:
  stdext::coroutine_handle<> continuation_{stdext::noop_coroutine()};
};

}  // namespace injectx::stdext::coro
