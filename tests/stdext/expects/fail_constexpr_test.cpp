// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expects.hpp"

#include <cstdlib>

constexpr int run() {
  injectx::stdext::expects(false);
  return EXIT_SUCCESS;
}

int main() {
  constexpr auto result = run();
  return result;
}
