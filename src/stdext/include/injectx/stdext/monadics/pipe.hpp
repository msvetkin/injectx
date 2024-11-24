// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <concepts>
#include <utility>

namespace injectx::stdext::monadics {

namespace details::_pipe {

struct tag {};

template<typename Action>
struct fn {
  template<typename F>
  struct action : Action {
    using tag_type = tag;

    F f;

    template<typename T>
    [[nodiscard]] constexpr decltype(auto) operator()(T &&t) const noexcept
      requires requires {
        Action::invoke(std::forward<T>(t), std::forward<F>(f));
      }
    {
      return Action::invoke(std::forward<T>(t), std::forward<F>(f));
    }
  };

  template<typename T, typename F>
  inline static constexpr auto invocable = requires {
    action<F>::template invoke(std::declval<T>(), std::declval<F>());
  };

  template<typename F>
  [[nodiscard]] constexpr decltype(auto) operator()(F &&f) const noexcept {
    return action<decltype(std::forward<F>(f))>{.f = std::forward<F>(f)};
  }
};

}  // namespace details::_pipe

template<typename Action>
using pipe = details::_pipe::fn<Action>;

template<typename T, typename A>
concept invocable = requires(T &&t, A &&a) {
  requires std::same_as<
      typename std::remove_cvref_t<A>::tag_type, details::_pipe::tag>;
  std::forward<A>(a)(std::forward<T>(t));
};

}  // namespace injectx::stdext::monadics

template<typename T, typename A>
[[nodiscard]] constexpr auto operator|(T &&t, A &&a) noexcept
  requires injectx::stdext::monadics::invocable<decltype(t), decltype(a)>
{
  return std::forward<A>(a)(std::forward<T>(t));
}
