// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/static_string.hpp"

#include <catch2/catch_test_macros.hpp>

#include <functional>

namespace injectx::stdext::tests {

TEST_CASE("from-const-char") {
  constexpr static_string s{"hello"};

  STATIC_REQUIRE(s.size() == 5);
  STATIC_REQUIRE(s[0] == 'h');
  STATIC_REQUIRE(s[1] == 'e');
  STATIC_REQUIRE(s[2] == 'l');
  STATIC_REQUIRE(s[3] == 'l');
  STATIC_REQUIRE(s[4] == 'o');
  STATIC_REQUIRE(s == static_string{"hello"});
  STATIC_REQUIRE(s != static_string{"gello"});
}

TEST_CASE("from-string-view") {
  constexpr std::string_view msg{"test"};
  constexpr auto s = STDEXT_AS_STATIC_STRING(msg);

  STATIC_REQUIRE(s.size() == msg.size());
  STATIC_REQUIRE(s[0] == 't');
  STATIC_REQUIRE(s[1] == 'e');
  STATIC_REQUIRE(s[2] == 's');
  STATIC_REQUIRE(s[3] == 't');
}

TEST_CASE("formatter") {
  constexpr auto s = std::invoke([] {
    constexpr static_string s{"hello"};
    std::array<char, s.size()> storage{0};
    fmt::format_to(storage.data(), FMT_COMPILE("{}"), s);
    return storage;
  });

  STATIC_REQUIRE(s.size() == 5);
  STATIC_REQUIRE(s[0] == 'h');
  STATIC_REQUIRE(s[1] == 'e');
  STATIC_REQUIRE(s[2] == 'l');
  STATIC_REQUIRE(s[3] == 'l');
  STATIC_REQUIRE(s[4] == 'o');
}

}  // namespace injectx::stdext::tests
