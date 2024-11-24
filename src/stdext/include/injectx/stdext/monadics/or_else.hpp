// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/monadics/pipe.hpp"

namespace injectx::stdext {

namespace monadics::details::_or_else {

struct action {
  template<typename T, typename F>
  [[nodiscard]] static constexpr decltype(auto) invoke(T &&t, F &&f) noexcept
    requires requires { or_else_as(std::forward<T>(t), std::forward<F>(f)); }
  {
    return or_else_as(std::forward<T>(t), std::forward<F>(f));
  }
};

using fn = monadics::pipe<action>;

}  // namespace monadics::details::_or_else

inline constexpr monadics::details::_or_else::fn or_else{};

}  // namespace injectx::stdext
