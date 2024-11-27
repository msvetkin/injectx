// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/type_name.hpp"

#include <catch2/catch_test_macros.hpp>

// skip stdext in wraping namespace since it effect result
namespace injectx::tests {

TEST_CASE("pod-type") {
  auto name = stdext::type_name<int>();
  REQUIRE(name == "int");
}

class MyClass {};

TEST_CASE("class") {
  constexpr auto name = stdext::type_name<MyClass>();
  STATIC_REQUIRE(name == "injectx::tests::MyClass");
}

class MyStruct {};

TEST_CASE("struct") {
  constexpr auto name = stdext::type_name<MyStruct>();
  STATIC_REQUIRE(name == "injectx::tests::MyStruct");
}

template<typename T>
class MyTemplate {};

TEST_CASE("template") {
  constexpr auto name = stdext::type_name<MyTemplate<int>>();
  STATIC_REQUIRE(name == "injectx::tests::MyTemplate<int>");
}

}  // namespace injectx::tests
