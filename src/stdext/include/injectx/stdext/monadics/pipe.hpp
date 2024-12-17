// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <concepts>
#include <utility>

namespace injectx::stdext::monadics {

namespace details::_pipe {

template<typename Action>
struct fn {
  template<typename Callback>
  struct pipeable_action : Action {
    template<typename C>
      requires std::constructible_from<Callback, C>
    constexpr pipeable_action(C &&cb)
        : cb_(std::forward<C>(cb)) {
    }

    template<typename T>
      requires requires {
        Action::invoke(std::declval<T>(), std::declval<Callback>());
      }
    [[nodiscard]] friend constexpr auto operator|(
        T &&t, const pipeable_action<Callback> &a) noexcept {
      return Action::invoke(std::forward<T>(t), std::forward<Callback>(a.cb_));
    }

   private:
    Callback cb_;
  };

  template<typename T, typename F>
  inline static constexpr auto invocable = requires {
    pipeable_action<F>::template invoke(std::declval<T>(), std::declval<F>());
  };

  template<typename Callback>
  [[nodiscard]] constexpr decltype(auto) operator()(
      Callback &&cb) const noexcept {
    return pipeable_action<decltype(cb)>{std::forward<Callback>(cb)};
  }
};

}  // namespace details::_pipe

template<typename Action>
using pipe = details::_pipe::fn<Action>;

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
