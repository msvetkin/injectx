// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/launch.hpp"

#include <fmt/core.h>

namespace injectx::core {

SetupTask<void> launch(Bundle bundle) noexcept {
  fmt::println("launch - 1");
  DependencyContainer dependencyContainer;
  std::vector<SetupTask<void>> setupTasks;
  setupTasks.reserve(bundle.size());

  for (const auto &module : bundle) {
    auto setupTask = module.setup(dependencyContainer);
    if (const auto res = setupTask.init(); !res.has_value()) {
      co_yield stdext::unexpected{res.error()};
    }

    setupTasks.push_back(std::move(setupTask));
  }

  fmt::println("launch - 2");
  co_yield {};
  fmt::println("launch - 3");

  for (auto &setupTask : setupTasks) {
    if (const auto res = setupTask.teardown(); !res.has_value()) {
      co_yield stdext::unexpected{res.error()};
    }
  }

  fmt::println("launch - 4");
  co_return;
}

}  // namespace injectx::core
