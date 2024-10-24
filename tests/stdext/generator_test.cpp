// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/generator.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <optional>
#include <vector>

namespace injectx::stdext::tests {

TEST_CASE("for-loop") {
  auto getValues = [](int max) -> generator<int> {
    for (int i = 0; i < max; ++i) {
      co_yield i;
    }
  };

  std::vector<int> values;
  for (auto v : getValues(10)) {
    values.push_back(v);
  }

  REQUIRE(values.size() == 10);
  REQUIRE(values == std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
}

TEST_CASE("for-loop-twice") {
  auto t = std::invoke(
      [](int max) -> generator<int> {
        for (int i = 0; i < max; ++i) {
          co_yield i;
        }
      },
      10);

  std::vector<int> values;
  for (auto v : t) {
    values.push_back(v);
  }

  for (auto v : t) {
    values.push_back(v);
  }

  REQUIRE(values.size() == 10);
}

TEST_CASE("optional") {
  using Optional = std::optional<int>;

  auto t = std::invoke([]() -> generator<Optional> {
    co_yield 10;
  });

  std::vector<Optional> values;
  for (auto v : t) {
    values.push_back(v);
  }

  REQUIRE(values.size() == 1);
  REQUIRE(values == std::vector{Optional{10}});
}

}  // namespace injectx::stdext::tests
