// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/ranges/views/enumerate.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <map>

namespace injectx::stdext::tests {

TEST_CASE("function-sequential") {
  constexpr std::array values = {0, 1, 2, 3, 4, 5};

  for (const auto [index, value] : ranges::views::enumerate(values)) {
    REQUIRE(index == static_cast<std::size_t>(value));
  }
}

TEST_CASE("function-associative") {
  const std::map values = {{
      std::pair{"a", 0},
      std::pair{"b", 1},
      std::pair{"c", 2},
  }};

  for (const auto [index, p] : ranges::views::enumerate(values)) {
    const auto [_, value] = p;
    REQUIRE(index == static_cast<std::size_t>(value));
  }
}

TEST_CASE("pipe-sequential") {
  constexpr std::array values = {0, 1, 2, 3, 4, 5};

  for (const auto [index, value] : values | ranges::views::enumerate) {
    REQUIRE(index == static_cast<std::size_t>(value));
  }
}

TEST_CASE("pipe-associative") {
  const std::map values = {{
      std::pair{"a", 0},
      std::pair{"b", 1},
      std::pair{"c", 2},
  }};

  for (const auto [index, p] : ranges::views::enumerate(values)) {
    const auto [_, value] = p;
    REQUIRE(index == static_cast<std::size_t>(value));
  }
}

}  // namespace injectx::stdext::tests
