// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <string_view>

namespace injectx::stdext {

template<typename T>
[[nodiscard]] consteval auto type_name() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
  constexpr std::string_view func = __FUNCSIG__;
#else
  constexpr std::string_view func = __PRETTY_FUNCTION__;
#endif

#if defined(_MSC_VER) && !defined(__clang__)
  const auto prefix = sizeof("auto __cdecl injectx::stdext::type_name<") - 1;
  const std::string_view suffix = ">(void) noexcept";
#elif defined(__clang__)
  const auto prefix = sizeof("auto injectx::stdext::type_name() [T = ") - 1;
  const std::string_view suffix = "]";
#elif defined(__GNUC__)
  const auto prefix =
      sizeof("constexpr auto injectx::stdext::type_name() [with T = ") - 1;
  const std::string_view suffix = "]";
#else
#  error "stdext::type_name is not supported";
#endif

  std::string_view begin = func.substr(prefix);

#if defined(_MSC_VER) && !defined(__clang__)
  if (begin.starts_with("struct ")) {
    begin.remove_prefix(sizeof("struct ") - 1);
  }
  if (begin.starts_with("class ")) {
    begin.remove_prefix(sizeof("class ") - 1);
  }
#endif

  return begin.substr(0, begin.find(suffix));
}

}  // namespace injectx::stdext
