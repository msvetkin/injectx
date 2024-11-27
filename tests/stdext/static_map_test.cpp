// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/static_map.hpp"

#include "injectx/stdext/monadics.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <string_view>

namespace injectx::stdext::tests {

TEST_CASE("initial-state") {
  constexpr static_map<std::string_view, int, 3> map;
  STATIC_REQUIRE(map.empty());
  STATIC_REQUIRE(map.size() == 0);
  STATIC_REQUIRE(map.max_size() == 3);
}

TEST_CASE("try-emplace") {
  using Map = static_map<std::string_view, int, 5>;
  using Pair = typename Map::value_type;

  constexpr auto p = std::invoke([] {
    Map map;

    auto emplace = [&](std::string_view k, int v) {
      return map.try_emplace(k, v) | stdext::transform([](auto &&it) {
               return *it;
             })
           | stdext::transform_error([](auto &&e) -> std::optional<Pair> {
               if (e.has_value()) {
                 return *e.value();
               }

               return std::nullopt;
             });
    };

    std::array ops{emplace("a", 1), emplace("a", 10), emplace("a", 2),
                   emplace("b", 3), emplace("c", 4),  emplace("d", 5),
                   emplace("f", 6), emplace("g", 7)};
    return std::pair{ops, map};
  });

  constexpr auto ops = p.first;
  STATIC_REQUIRE(ops.size() == 8);
  STATIC_REQUIRE(ops[0].value() == Pair{"a", 1});
  STATIC_REQUIRE(ops[1].error().value() == Pair{"a", 1});
  STATIC_REQUIRE(ops[3].value() == Pair{"b", 3});
  STATIC_REQUIRE(ops[4].value() == Pair{"c", 4});
  STATIC_REQUIRE(ops[5].value() == Pair{"d", 5});
  STATIC_REQUIRE(ops[6].value() == Pair{"f", 6});
  STATIC_REQUIRE(ops[7].error().has_value() == false);

  constexpr auto map = p.second;
  STATIC_REQUIRE(map.size() == 5);
  STATIC_REQUIRE(map.max_size() == 5);
  STATIC_REQUIRE(map.empty() == false);
}

TEST_CASE("find") {
  static constexpr auto map = std::invoke([] {
    static_map<std::string_view, int, 3> map;
    (void)map.try_emplace("b", 3);
    (void)map.try_emplace("a", 2);
    (void)map.try_emplace("c", 4);
    (void)map.try_emplace("a", 1);
    return map;
  });

  STATIC_REQUIRE(map.size() == 3);
  STATIC_REQUIRE(map.max_size() == 3);

  constexpr auto b = map.find("b");
  STATIC_REQUIRE(b != map.end());
  STATIC_REQUIRE(b->first == "b");
  STATIC_REQUIRE(b->second == 3);

  constexpr auto a = map.find("a");
  STATIC_REQUIRE(a != map.end());
  STATIC_REQUIRE(a->first == "a");
  STATIC_REQUIRE(a->second == 2);

  constexpr auto c = map.find("c");
  STATIC_REQUIRE(c != map.end());
  STATIC_REQUIRE(c->first == "c");
  STATIC_REQUIRE(c->second == 4);
}

TEST_CASE("for-loop") {
  using Map = static_map<std::string_view, int, 3>;

  constexpr auto elems = std::invoke([] {
    Map map;

    (void)map.try_emplace("a", 3);
    (void)map.try_emplace("b", 0);
    (void)map.try_emplace("c", 1);
    (void)map.try_emplace("g", 2);

    std::array<std::string_view, 3> keys;
    std::array<int, 3> values;
    std::size_t index{0};
    for (const auto p : map) {
      keys[index] = p.first;
      values[index++] = p.second;
    }

    return std::pair{keys, values};
  });

  constexpr auto keys = elems.first;
  STATIC_REQUIRE(keys.size() == 3);
  STATIC_REQUIRE(keys.at(0) == std::string_view{"a"});
  STATIC_REQUIRE(keys.at(1) == std::string_view{"b"});
  STATIC_REQUIRE(keys.at(2) == std::string_view{"c"});

  constexpr auto values = elems.second;
  STATIC_REQUIRE(values.size() == 3);
  STATIC_REQUIRE(values.at(0) == 3);
  STATIC_REQUIRE(values.at(1) == 0);
  STATIC_REQUIRE(values.at(2) == 1);
}

}  // namespace injectx::stdext::tests
