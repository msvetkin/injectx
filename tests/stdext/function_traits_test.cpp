// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/function_traits.hpp"

#include <catch2/catch_test_macros.hpp>

#include <concepts>

namespace injectx::stdext::tests {

TEST_CASE("lambda-no-args") {
  auto lambda = [] {
  };
  using LambdaTraits = stdext::function_traits<decltype(lambda)>;

  STATIC_REQUIRE(LambdaTraits::args_count == 0);
  STATIC_REQUIRE(std::same_as<LambdaTraits::return_type, void>);
}

TEST_CASE("lambda-with-args") {
  auto lambda = [](int a, int b) {
    return a + b;
  };
  using LambdaTraits = stdext::function_traits<decltype(lambda)>;

  STATIC_REQUIRE(LambdaTraits::args_count == 2);
  STATIC_REQUIRE(std::same_as<LambdaTraits::arg<0>, int>);
  STATIC_REQUIRE(std::same_as<LambdaTraits::arg<1>, int>);
  STATIC_REQUIRE(std::same_as<LambdaTraits::return_type, int>);
}

TEST_CASE("lambda-with-args-noexcept") {
  auto lambda = [](int a, int b) noexcept {
    return a + b;
  };
  using LambdaTraits = stdext::function_traits<decltype(lambda)>;

  STATIC_REQUIRE(LambdaTraits::args_count == 2);
  STATIC_REQUIRE(std::same_as<LambdaTraits::arg<0>, int>);
  STATIC_REQUIRE(std::same_as<LambdaTraits::arg<1>, int>);
  STATIC_REQUIRE(std::same_as<LambdaTraits::return_type, int>);
}

void foo1() {
}

TEST_CASE("free-function-no-args") {
  using FunctionTraits = stdext::function_traits<decltype(&foo1)>;

  STATIC_REQUIRE(FunctionTraits::args_count == 0);
  STATIC_REQUIRE(std::same_as<FunctionTraits::return_type, void>);
}

void foo2(float /*a*/, int /*b*/) {
}

TEST_CASE("free-function-with-args") {
  using FunctionTraits = stdext::function_traits<decltype(&foo2)>;

  STATIC_REQUIRE(FunctionTraits::args_count == 2);
  STATIC_REQUIRE(std::same_as<FunctionTraits::arg<0>, float>);
  STATIC_REQUIRE(std::same_as<FunctionTraits::arg<1>, int>);
  STATIC_REQUIRE(std::same_as<FunctionTraits::return_type, void>);
}

int *foo3(float /*a*/) noexcept {
  return nullptr;
}

TEST_CASE("free-function-noexcept-with-arg") {
  using FunctionTraits = stdext::function_traits<decltype(&foo3)>;

  STATIC_REQUIRE(FunctionTraits::args_count == 1);
  STATIC_REQUIRE(std::same_as<FunctionTraits::arg<0>, float>);
  STATIC_REQUIRE(std::same_as<FunctionTraits::return_type, int *>);
}

TEST_CASE("method-const-no-args") {
  struct Foo {
    int boo() const {
      return 10;
    }
  };

  using MethodTraits = stdext::function_traits<decltype(&Foo::boo)>;

  STATIC_REQUIRE(MethodTraits::args_count == 0);
  STATIC_REQUIRE(std::same_as<MethodTraits::return_type, int>);
}

TEST_CASE("method-const-noexcept-no-args") {
  struct Foo {
    void boo() const noexcept {
    }
  };

  using MethodTraits = stdext::function_traits<decltype(&Foo::boo)>;

  STATIC_REQUIRE(MethodTraits::args_count == 0);
  STATIC_REQUIRE(std::same_as<MethodTraits::return_type, void>);
}

TEST_CASE("method-with-arg") {
  struct Foo {
    void boo(int /*a*/) {
    }
  };

  using MethodTraits = stdext::function_traits<decltype(&Foo::boo)>;

  STATIC_REQUIRE(MethodTraits::args_count == 1);
  STATIC_REQUIRE(std::same_as<MethodTraits::arg<0>, int>);
  STATIC_REQUIRE(std::same_as<MethodTraits::return_type, void>);
}

TEST_CASE("method-noexcept-with-arg") {
  struct Foo {
    float boo() noexcept {
      return 10.0;
    }
  };

  using MethodTraits = stdext::function_traits<decltype(&Foo::boo)>;

  STATIC_REQUIRE(MethodTraits::args_count == 0);
  STATIC_REQUIRE(std::same_as<MethodTraits::return_type, float>);
}

}  // namespace injectx::stdext::tests
