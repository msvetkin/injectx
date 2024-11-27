// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/static_queue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <string_view>
#include <tuple>

namespace injectx::stdext::tests {

TEST_CASE("initial-state") {
  constexpr static_queue<int, 3> queue;
  STATIC_REQUIRE(queue.size() == 0);
  STATIC_REQUIRE(queue.max_size() == 3);
  STATIC_REQUIRE(queue.front().has_value() == false);
}

struct Some {
  int value{};
};

constexpr const Some &unwrap_as(
    const std::reference_wrapper<const Some> &rw) noexcept {
  return rw.get();
}

TEST_CASE("push") {
  constexpr auto p = std::invoke([] {
    static_queue<Some, 5> queue;

    auto ops = std::invoke(
        [&queue](auto... args) {
          return std::array{queue.push({args})...};
        },
        7, 5, 3, 1, 2, 3, 4, 5);

    return std::pair{ops, queue};
  });

  constexpr auto ops = p.first;
  STATIC_REQUIRE(ops[0].has_value());
  STATIC_REQUIRE(ops[1].has_value());
  STATIC_REQUIRE(ops[2].has_value());
  STATIC_REQUIRE(ops[3].has_value());
  STATIC_REQUIRE(ops[4].has_value());
  constexpr auto queue = p.second;
  STATIC_REQUIRE(queue.size() == 5);
  STATIC_REQUIRE(queue.front().has_value());
  STATIC_REQUIRE(queue.front()->value == 7);
}

TEST_CASE("push-and-pop") {
  constexpr auto t = std::invoke([] {
    static_queue<int, 5> queue;

    auto pushOps = std::invoke(
        [&queue](auto... args) {
          return std::array{queue.push(args)...};
        },
        3, 4, 5);

    return std::make_tuple(
        pushOps,
        std::array{
            queue.pop(),
            queue.pop(),
            queue.pop(),
            queue.pop(),
            queue.pop(),
            queue.pop(),
        },
        queue);
  });

  constexpr auto pushOps = std::get<0>(t);
  STATIC_REQUIRE(pushOps.size() == 3);
  STATIC_REQUIRE(pushOps[0].has_value());
  STATIC_REQUIRE(pushOps[1].has_value());
  STATIC_REQUIRE(pushOps[2].has_value());

  constexpr auto popOps = std::get<1>(t);
  STATIC_REQUIRE(popOps.size() == 6);
  STATIC_REQUIRE(popOps[0].has_value());
  STATIC_REQUIRE(popOps[1].has_value());
  STATIC_REQUIRE(popOps[2].has_value());
  STATIC_REQUIRE(popOps[3].has_value() == false);
  STATIC_REQUIRE(popOps[3].error() == std::string_view{"queue is empty"});
  STATIC_REQUIRE(popOps[4].has_value() == false);
  STATIC_REQUIRE(popOps[4].error() == std::string_view{"queue is empty"});
  STATIC_REQUIRE(popOps[5].has_value() == false);
  STATIC_REQUIRE(popOps[5].error() == std::string_view{"queue is empty"});

  constexpr auto queue = std::get<2>(t);
  REQUIRE(queue.size() == 0);
  STATIC_REQUIRE(queue.front().has_value() == false);
  STATIC_REQUIRE(queue.front().error() == std::string_view{"queue is empty"});
}

}  // namespace injectx::stdext::tests
