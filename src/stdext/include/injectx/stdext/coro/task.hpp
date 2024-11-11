// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/coro/promise.hpp"
#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"

#include <string_view>
#include <utility>

namespace injectx::stdext::coro {

template <typename T, template <typename> typename Promise>
class [[nodiscard]] task;

namespace details::_task {

template <typename T>
class promise_base : public promise_storage<T> {
 public:
  template <typename U = T>
  void return_value(U&& value) noexcept {
    promise_storage<T>::set(std::forward<U>(value));
  }
};

template <>
class promise_base<void> : public promise_storage<void> {
 public:
  void return_void() const noexcept {
  }
};

template <typename TaskPromise>
class [[nodiscard]] Awaitable {
 public:
  using handle_type = stdext::coroutine_handle<TaskPromise>;

  explicit Awaitable(handle_type handle) noexcept
      : handle_(handle) {
  }

  bool await_ready() const noexcept {
    return !handle_ || handle_.done();
  }

  template <typename Promise>
  auto await_suspend(
      stdext::coroutine_handle<Promise> awaitingHandle) noexcept {
    return handle_.promise().suspend(awaitingHandle, handle_);
  }

  decltype(auto) await_resume() {
    return handle_.promise().result();
  }

 private:
  handle_type handle_;
};

template <typename T>
class promise : public promise_base<T>, public promise_continuation {
 public:
  task<T, promise> get_return_object() noexcept;
};

}  // namespace details::_task

template <typename T,
          template <typename> typename Promise = details::_task::promise>
class [[nodiscard]] task {
 public:
  using promise_type = Promise<T>;
  using handle_type = stdext::coroutine_handle<promise_type>;
  using value_type = typename promise_type::value_type;

  explicit task(handle_type handle) noexcept
      : handle_(std::move(handle)) {
  }

  task(task&& that) noexcept
      : handle_(std::exchange(that.handle_, nullptr)) {
  }

  task& operator=(task&& other) noexcept {
    if (std::addressof(other) != this) {
      if (handle_) {
        handle_.destroy();
      }
      handle_ = std::exchange(other.handle_, {});
    }

    return *this;
  }

  task(const task&) = delete;
  task& operator=(const task&) = delete;

  ~task() {
    if (handle_) {
      handle_.destroy();
    }
  }

  stdext::expected<bool, std::string_view> is_ready() const {
    if (!handle_) {
      return stdext::unexpected{"coroutine handle is null"};
    }

    return handle_.done();
  }

  stdext::expected<void, std::string_view> resume() {
    using Expected = stdext::expected<void, std::string_view>;

    return is_ready() | and_then([this](const bool ready) -> Expected {
      if (ready) {
        return stdext::unexpected{"coroutine is in the final suspend point"};
      }

      handle_.resume();
      return {};
    });
  }

  [[nodiscard]] promise_type& promise() noexcept {
    return handle_.promise();
  };

  auto operator co_await() const noexcept {
    return details::_task::Awaitable<promise_type>{handle_};
  }

 private:
  handle_type handle_;
};

namespace details::_task {

template <typename T>
[[nodiscard]] task<T, promise> promise<T>::get_return_object() noexcept {
  return task<T, promise>{
      stdext::coroutine_handle<promise>::from_promise(*this)};
}

}  // namespace details::_task

}  // namespace injectx::stdext::coro
