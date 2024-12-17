// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/monadics/pipe.hpp"

namespace injectx::stdext {

namespace monadics::details::_and_then {

template<typename Callback>
struct action {
  Callback callback;

  template<typename T>
  [[nodiscard]] friend constexpr auto operator|(T &&t, action &&a) noexcept
    requires requires {
      and_then_as(std::forward<T>(t), std::forward<Callback>(a.callback));
    }
  {
    return and_then_as(std::forward<T>(t), std::forward<Callback>(a.callback));
  }
};

using fn = monadics::pipe_fn<action>;

}  // namespace monadics::details::_and_then

inline constexpr monadics::details::_and_then::fn and_then{};

}  // namespace injectx::stdext
