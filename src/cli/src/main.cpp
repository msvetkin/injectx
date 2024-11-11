// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/core/version.hpp"

#include <fmt/core.h>

int main(int /*argc*/, char* /*argv*/[]) {
  fmt::print("injectx version: {}\n", injectx::core::version());
  return 0;
}
