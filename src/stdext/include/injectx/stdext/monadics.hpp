// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <concepts>
#include <utility>

namespace injectx::stdext {

namespace details::_monadics {

template<typename F>
struct action {
  F f;
};

template<template<typename> typename Action>
struct fn {
  template<typename F>
  [[nodiscard]] constexpr decltype(auto) operator()(F &&f) const noexcept {
    return Action<decltype(std::forward<F>(f))>{std::forward<F>(f)};
  }
};

}  // namespace details::_monadics

template<typename F>
struct and_then_action : details::_monadics::action<F> {};

inline constexpr details::_monadics::fn<and_then_action> and_then{};

template<typename T, typename F>
concept and_thenable = requires(T t, F f) {
  typename std::remove_cvref_t<T>::value_type;
  { and_then_as(std::forward<T>(t), std::forward<F>(f)) };
};

template<typename F>
struct transform_action : details::_monadics::action<F> {};

inline constexpr details::_monadics::fn<transform_action> transform{};

template<typename T, typename F>
concept transformable = requires(T t, F f) {
  typename std::remove_cvref_t<T>::value_type;
  { transform_as(std::forward<T>(t), std::forward<F>(f)) };
};

}  // namespace injectx::stdext

template<typename T, typename F>
[[nodiscard]] constexpr auto operator|(
    T &&t, const injectx::stdext::and_then_action<F> &action) noexcept
  requires injectx::stdext::and_thenable<decltype(std::forward<T>(t)), F>
{
  return and_then_as(std::forward<T>(t), std::forward<F>(action.f));
}

template<typename T, typename F>
[[nodiscard]] constexpr auto operator|(
    T &&t, const injectx::stdext::transform_action<F> &action) noexcept
  requires injectx::stdext::transformable<decltype(std::forward<T>(t)), F>
{
  return transform_as(std::forward<T>(t), std::forward<F>(action.f));
}
