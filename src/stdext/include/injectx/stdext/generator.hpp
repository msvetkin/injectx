// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/coro/task.hpp"

#include <concepts>
#include <iterator>

namespace injectx::stdext {

template<typename T>
class [[nodiscard]] generator;

namespace details::_generator {

template<typename T>
concept HasValueType = requires { typename T::value_type; };

template<typename T>
constexpr auto defaultValueType() {
  if constexpr (HasValueType<T>) {
    return std::type_identity<typename T::value_type>{};
  } else {
    return std::type_identity<T>{};
  }
}

template<typename T>
using DefaultValueType = typename decltype(defaultValueType<T>())::type;

template<typename T>
class promise : public coro::promise_storage<T>,
                public coro::promise_continuation {
 public:
  [[nodiscard]] generator<T> get_return_object() noexcept;

  suspend_always final_suspend() noexcept {
    return {};
  }

  template<typename From = DefaultValueType<T>>
    requires std::constructible_from<T, From>
  coro::resume_continuation yield_value(From&& from) {
    coro::promise_storage<T>::set(std::forward<From>(from));
    return {};
  }

  void return_void() const noexcept {
  }
};

template<typename Task>
class [[nodiscard]] iterator {
 public:
  using value_type = typename Task::promise_type::value_type;
  using difference_type = std::ptrdiff_t;

  explicit iterator(Task& task) noexcept
      : task_(task) {
  }

  iterator& operator++() {
    (void)task_.resume();
    return *this;
  }

  [[nodiscard]] value_type operator*() const noexcept {
    return task_.promise().result();
  }

  [[nodiscard]] friend bool operator==(
      const iterator& it, std::default_sentinel_t) {
    const auto res = it.task_.is_ready();
    return res.has_value() && res.value();
  }

 private:
  Task& task_;
};

}  // namespace details::_generator

template<typename T>
class [[nodiscard]] generator {
  using Task = coro::task<T, details::_generator::promise>;

 public:
  using value_type = typename Task::value_type;
  using promise_type = typename Task::promise_type;
  using handle_type = typename stdext::coroutine_handle<promise_type>;

  explicit generator(handle_type handle) noexcept
      : task_(std::move(handle)) {
  }

  auto begin() noexcept {
    auto it = details::_generator::iterator{task_};
    ++it;
    return it;
  }

  [[nodiscard]] auto end() const noexcept {
    return std::default_sentinel;
  }

 private:
  Task task_;
};

namespace details::_generator {

template<typename T>
[[nodiscard]] generator<T> promise<T>::get_return_object() noexcept {
  return generator<T>{stdext::coroutine_handle<promise>::from_promise(*this)};
}

}  // namespace details::_generator

}  // namespace injectx::stdext
