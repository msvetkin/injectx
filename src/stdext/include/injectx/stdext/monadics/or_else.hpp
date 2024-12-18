// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/monadics/pipe.hpp"

namespace injectx::stdext {

namespace monadics::details::_or_else {

template<typename Callback>
struct action {
  Callback callback;

  template<typename T>
  [[nodiscard]] friend constexpr auto operator|(T &&t, action &&a) noexcept
    requires requires {
      or_else_as(std::forward<T>(t), std::declval<Callback>());
    }
  {
    return or_else_as(std::forward<T>(t), std::forward<Callback>(a.callback));
  }
};

using fn = monadics::pipe_fn<action>;

}  // namespace monadics::details::_or_else

inline constexpr monadics::details::_or_else::fn or_else{};

}  // namespace injectx::stdext
