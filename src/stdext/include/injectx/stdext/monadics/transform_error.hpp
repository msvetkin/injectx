// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/monadics/pipe.hpp"

namespace injectx::stdext {

namespace monadics::details::_transform_error {

struct action {
  template<typename T, typename F>
  [[nodiscard]] static constexpr decltype(auto) invoke(T &&t, F &&f) noexcept
    requires requires {
      transform_error_as(std::forward<T>(t), std::forward<F>(f));
    }
  {
    return transform_error_as(std::forward<T>(t), std::forward<F>(f));
  }
};

using fn = monadics::pipe<action>;

}  // namespace monadics::details::_transform_error

inline constexpr monadics::details::_transform_error::fn transform_error{};

}  // namespace injectx::stdext
