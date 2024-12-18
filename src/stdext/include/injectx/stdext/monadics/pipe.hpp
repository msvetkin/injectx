// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <utility>

namespace injectx::stdext::monadics {

template<template<typename> typename Action>
struct pipe_fn {
  template<typename T, typename F>
  inline static constexpr auto invocable =
      requires { std::declval<T>() | std::declval<Action<F>>(); };

  template<typename Callback>
  [[nodiscard]] constexpr decltype(auto) operator()(
      Callback &&cb) const noexcept {
#if defined(__clang_major__) && __clang_major__ < 16
    // only clang16 can automatically deduce type
    // but for some reason tst-expected-monadics-transform does not work with
    // this.
    return Action<decltype(cb)>{std::forward<Callback>(cb)};
#else
    return Action{std::forward<Callback>(cb)};
#endif
  }
};

}  // namespace injectx::stdext::monadics
