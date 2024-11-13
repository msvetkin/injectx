// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#if __has_include(<coroutine>)
#  include <coroutine>

namespace injectx::stdext {

template<typename Return, typename... Args>
using coroutine_traits = std::coroutine_traits<Return, Args...>;

template<typename Promise = void>
using coroutine_handle = std::coroutine_handle<Promise>;

using noop_coroutine_handle = std::noop_coroutine_handle;

inline noop_coroutine_handle noop_coroutine() noexcept {
  return std::noop_coroutine();
}

using suspend_never = std::suspend_never;

using suspend_always = std::suspend_always;

}  // namespace injectx::stdext

#elif __has_include(<experimental/coroutine>)
#  include <experimental/coroutine>

namespace injectx::stdext {

template<typename Return, typename... Args>
using coroutine_traits = std::experimental::coroutine_traits<Return, Args...>;

template<typename Promise = void>
using coroutine_handle = std::experimental::coroutine_handle<Promise>;

using noop_coroutine_handle = std::experimental::noop_coroutine_handle;

inline noop_coroutine_handle noop_coroutine() noexcept {
  return std::experimental::noop_coroutine();
}

using suspend_never = std::experimental::suspend_never;

using suspend_always = std::experimental::suspend_always;

}  // namespace injectx::stdext

#else
#  error "coroutine is unsupported"
#endif
