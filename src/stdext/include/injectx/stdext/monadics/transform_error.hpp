// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/monadics/pipe.hpp"

namespace injectx::stdext {

namespace monadics::details::_transform_error {

template<typename Callback>
struct action {
  Callback callback;

  template<typename T>
  [[nodiscard]] friend constexpr auto operator|(T &&t, action &&a) noexcept
    requires requires {
      transform_error_as(
          std::forward<T>(t), std::forward<Callback>(a.callback));
    }
  {
    return transform_error_as(
        std::forward<T>(t), std::forward<Callback>(a.callback));
  }
};

using fn = pipe_fn<action>;

}  // namespace monadics::details::_transform_error

inline constexpr monadics::details::_transform_error::fn transform_error{};

}  // namespace injectx::stdext
