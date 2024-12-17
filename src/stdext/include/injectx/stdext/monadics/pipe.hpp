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
    return Action{std::forward<Callback>(cb)};
  }
};

}  // namespace injectx::stdext::monadics
