// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <tuple>
#include <type_traits>

namespace injectx::stdext {

template <typename T>
struct function_traits;

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
  static constexpr std::size_t args_count = sizeof...(Args);

  using return_type = R;

  template <std::size_t Index>
  using arg = std::tuple_element_t<Index, std::tuple<Args...>>;

  template <std::size_t Index, typename None>
  struct arg_or_impl {
    [[nodiscard]] static constexpr auto extractType() noexcept {
      if constexpr (args_count < Index + 1) {
        return std::type_identity<None>{};
      } else {
        return std::type_identity<arg<Index>>{};
      }
    }

    using type = typename decltype(extractType())::type;
  };

  template <std::size_t Index, typename None>
  using arg_or = typename arg_or_impl<Index, None>::type;
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...) noexcept>
    : function_traits<R (*)(Args...)> {};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R (*)(Args...)> {
  using class_type = C;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const noexcept>
    : public function_traits<R (*)(Args...)> {
  using class_type = C;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R (*)(Args...)> {
  using class_type = C;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) noexcept>
    : public function_traits<R (*)(Args...)> {
  using class_type = C;
};

}  // namespace injectx::stdext
