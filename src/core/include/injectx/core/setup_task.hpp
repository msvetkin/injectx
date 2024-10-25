// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/generator.hpp"
#include "injectx/stdext/monadics.hpp"

#include <string>
#include <type_traits>
#include <variant>

namespace injectx::core {

namespace details::_setup_task {

template<typename T>
concept VoidOrStruct = std::is_void_v<T> || std::is_class_v<T>;

}  // namespace details::_setup_task

template<details::_setup_task::VoidOrStruct T>
class SetupTask {
  using Value = std::conditional_t<std::is_void_v<T>, std::monostate, T>;
  using Expected = stdext::expected<Value, std::string>;
  using Generator = stdext::generator<Expected>;

 public:
  using value_type = T;
  using promise_type = typename Generator::promise_type;

  /*implicit*/ SetupTask(Generator &&generator) noexcept
      : generator_(std::forward<Generator>(generator)) {
  }

  [[nodiscard]] stdext::expected<value_type, std::string> init() noexcept {
    if (initialized_) {
      return stdext::unexpected{"SetupTask has been already inialized"};
    }

    initialized_ = true;
    auto it = generator_.begin();
    if (it == generator_.end()) {
      return stdext::unexpected{"SetupTask is missing co_yield"};
    }

    if constexpr (std::is_void_v<value_type>) {
      return (*it) | stdext::transform([](auto) {});
    } else {
      return *it;
    }
  }

  [[nodiscard]] stdext::expected<void, std::string> teardown() noexcept {
    if (!initialized_) {
      return stdext::unexpected{"SetupTask task has not been inialized yet"};
    }

    if (auto it = generator_.begin(); it != generator_.end()) {
      const auto res = (*it);
      if (res.has_value() == false) {
        return stdext::unexpected{res.error()};
      }

      return stdext::unexpected{"SetupTask co_yield twice with Provides{}"};
    }

    return {};
  }

 private:
  bool initialized_{false};
  Generator generator_;
};

namespace details::_setup_task {

template<typename T>
inline constexpr bool IsSetupTask = false;

template<typename T>
inline constexpr bool IsSetupTask<SetupTask<T>> = true;

}  // namespace details::_setup_task

template<typename T>
concept IsSetupTask = details::_setup_task::IsSetupTask<std::remove_cvref_t<T>>;

}  // namespace injectx::core
