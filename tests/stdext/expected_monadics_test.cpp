// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <any>

namespace injectx::stdext::tests {

struct LookLikeExpected {
  using error_type = std::string_view;
};

template<typename T, typename R = T>
struct Monadics {
  using Value = decltype([](auto v) -> R {
    return v;
  });

  using LValueRef = decltype([](T &v) -> R {
    return v;
  });

  using ConstLValueRef = decltype([](const T &v) -> R {
    return v;
  });

  using RValueRef = decltype([](T &&v) -> R {
    return v;
  });

  using ConstRValueRef = decltype([](const T &&v) -> R {
    return v;
  });

  using LikeExpected = decltype([](T) {
    return LookLikeExpected{};
  });
};

class Some {
 public:
  Some &operator++() {
    ++value_;
    return *this;
  }

  Some &operator+(int value) {
    value_ += value;
    return *this;
  }

  operator int() const {
    return value_;
  }

 private:
  int value_{};
};

TEMPLATE_TEST_CASE("and-thenable-value", "", int, Some) {
  using Expected = expected<TestType, std::string_view>;
  using M = Monadics<TestType, expected<std::any, std::string_view>>;
  using Value = typename M::Value;
  using LValueRef = typename M::LValueRef;
  using RValueRef = typename M::RValueRef;
  using ConstLValueRef = typename M::ConstLValueRef;
  using ConstRValueRef = typename M::ConstRValueRef;
  using LikeExpected = typename M::LikeExpected;

  STATIC_REQUIRE(and_thenable<Expected &, Value>);
  STATIC_REQUIRE(and_thenable<Expected &, LValueRef>);
  STATIC_REQUIRE(and_thenable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(and_thenable<Expected &, RValueRef> == false);
  STATIC_REQUIRE(and_thenable<Expected &, ConstRValueRef> == false);

  STATIC_REQUIRE(and_thenable<const Expected &, Value>);
  STATIC_REQUIRE(and_thenable<const Expected &, LValueRef> == false);
  STATIC_REQUIRE(and_thenable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(and_thenable<const Expected &, RValueRef> == false);
  STATIC_REQUIRE(and_thenable<const Expected &, ConstRValueRef> == false);

  STATIC_REQUIRE(and_thenable<Expected &&, Value>);
  STATIC_REQUIRE(and_thenable<Expected &&, LValueRef> == false);
  STATIC_REQUIRE(and_thenable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(and_thenable<Expected &&, RValueRef>);
  STATIC_REQUIRE(and_thenable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(and_thenable<const Expected &&, Value>);
  STATIC_REQUIRE(and_thenable<const Expected &&, LValueRef> == false);
  STATIC_REQUIRE(and_thenable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(and_thenable<const Expected &&, RValueRef> == false);
  STATIC_REQUIRE(and_thenable<const Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(and_thenable<Expected &, LikeExpected> == false);
}

TEST_CASE("and-thenable-void") {
  using Expected = expected<void, std::string_view>;
  using F = decltype([]() -> expected<int, std::string_view> {
    return 1;
  });
  using Value = decltype([](auto) -> expected<int, std::string_view> {
    return 1;
  });
  using LikeExpected = decltype([]() {
    return LookLikeExpected{};
  });

  STATIC_REQUIRE(and_thenable<Expected &, F>);
  STATIC_REQUIRE(and_thenable<Expected &, Value> == false);

  STATIC_REQUIRE(and_thenable<const Expected &, F>);
  STATIC_REQUIRE(and_thenable<const Expected &, Value> == false);

  STATIC_REQUIRE(and_thenable<Expected &&, F>);
  STATIC_REQUIRE(and_thenable<Expected &&, Value> == false);

  STATIC_REQUIRE(and_thenable<const Expected &&, F>);
  STATIC_REQUIRE(and_thenable<const Expected &&, Value> == false);

  STATIC_REQUIRE(and_thenable<Expected &, LikeExpected> == false);
}

template<typename T>
expected<std::any, std::string_view> to_any(T &v) {
  ++v;
  return T{v} + 10;
}

TEMPLATE_TEST_CASE("and-then-value", "", int, Some) {
  using Expected = expected<TestType, std::string_view>;

  const auto initWithError = GENERATE(false, true);

  auto r1 = initWithError ? Expected{unexpected{"foo"}} : Expected{TestType{}};
  auto r2 = r1 | and_then(to_any<TestType>);
  auto r3 = r2 | and_then([](std::any) -> expected<void, std::string_view> {
              return {};
            });

  if (initWithError) {
    REQUIRE(r1.has_value() == false);
    REQUIRE(r2.has_value() == false);
    REQUIRE(r3.has_value() == false);
  } else {
    REQUIRE(r1.value() == 1);
    REQUIRE(std::any_cast<TestType>(r2.value()) == 11);
    REQUIRE(r3.has_value());
    STATIC_REQUIRE(std::is_void_v<typename decltype(r3)::value_type>);
  }
}

}  // namespace injectx::stdext::tests
