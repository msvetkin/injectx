// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/core/dependency_container.hpp"
#include "injectx/core/manifest.hpp"
#include "injectx/core/setup_task.hpp"
#include "injectx/stdext/expected.hpp"

namespace injectx::core {

namespace details::_module {

template<auto setup>
[[nodiscard]] auto invoke(DependencyContainer *dependencyContainer) noexcept {
  using STraits = SetupTraits<setup>;

  if constexpr (std::same_as<typename STraits::Requires, std::monostate>) {
    using Expected = stdext::expected<typename STraits::Result, std::string>;
    return Expected{setup()};
  } else {
    return dependencyContainer->resolve<typename STraits::Requires>()
         | stdext::transform(setup);
  }
}

template<auto setup>
[[nodiscard]] SetupTask<void> makeSetupTask(
    DependencyContainer *dependencyContainer) noexcept {
  auto setupTask = invoke<setup>(dependencyContainer);

  co_yield setupTask | stdext::and_then([](auto &task) {
    return task.init();
  }) | stdext::and_then([&dependencyContainer](auto &&...provides) {
    if constexpr (sizeof...(provides) == 0) {
      return stdext::expected<void, std::string>{};
    } else {
      return dependencyContainer->provide(
          std::forward<decltype(provides)>(provides)...);
    }
  }) | stdext::transform([] {
    return std::monostate{};
  });

  if (const auto res = setupTask->teardown(); !res.has_value()) {
    co_yield stdext::unexpected{res.error()};
  }

  co_return;
}

struct vtable {
  SetupTask<void> (*setup)(DependencyContainer &dependencyContainer);
};

template<auto setup>
inline constexpr vtable vtableFor = {
    .setup = [](DependencyContainer &dependencyContainer) {
      return makeSetupTask<setup>(&dependencyContainer);
    }};

}  // namespace details::_module

class Module {
 public:
  constexpr Module() = default;

  constexpr explicit Module(
      const details::_module::vtable *vtable, Manifest manifest)
      : vtable_(vtable),
        manifest_(manifest) {
  }

  [[nodiscard]] SetupTask<void> setup(
      DependencyContainer &dependencyContainer) const noexcept {
    return vtable_->setup(dependencyContainer);
  }

  constexpr std::string_view name() const noexcept {
    return manifest_.name();
  }

 private:
  const details::_module::vtable *vtable_{nullptr};
  Manifest manifest_{nullptr};
};

template<IsSetupFunction auto setup>
[[nodiscard]] constexpr auto makeModule() noexcept {
  return makeManifest<setup>() | stdext::transform([](auto manifest) {
           return Module{&details::_module::vtableFor<setup>, manifest};
         });
}

}  // namespace injectx::core
