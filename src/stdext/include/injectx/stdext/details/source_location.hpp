// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

namespace injectx::stdext::details::_source_location {

struct source_location {
  [[nodiscard]] static constexpr source_location current(
      const char* file = __builtin_FILE(),
      const char* func = __builtin_FUNCTION(),
      uint_least32_t line = __builtin_LINE(),
      uint_least32_t col = 0) noexcept {
    source_location loc;
    loc.file_ = file;
    loc.func_ = func;
    loc.line_ = line;
    loc.col_ = col;
    return loc;
  }

  constexpr source_location() noexcept = default;

  [[nodiscard]] constexpr uint_least32_t line() const noexcept {
    return line_;
  }

  [[nodiscard]] constexpr uint_least32_t column() const noexcept {
    return col_;
  }

  [[nodiscard]] constexpr const char* file_name() const noexcept {
    return file_;
  }

  [[nodiscard]] constexpr const char* function_name() const noexcept {
    return func_;
  }

 private:
  const char* file_{nullptr};
  const char* func_{nullptr};
  uint_least32_t line_{};
  uint_least32_t col_{};
};

}  // namespace injectx::stdext::details::_source_location
