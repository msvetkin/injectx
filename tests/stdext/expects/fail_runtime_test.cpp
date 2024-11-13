// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expects.hpp"

#include <cstdlib>

int main() {
  injectx::stdext::expects(false, "something is wrong");
  return EXIT_SUCCESS;
}
