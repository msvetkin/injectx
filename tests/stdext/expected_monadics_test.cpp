// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <any>

namespace {

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

template<typename R>
struct Monadics<void, R> {
  using Value = decltype([]() -> R {
    return 1;
  });
  using LValueRef = Value;
  using ConstLValueRef = Value;
  using RValueRef = Value;
  using ConstRValueRef = Value;
  using LikeExpected = decltype([]() {
    return LookLikeExpected{};
  });
};

}  // namespace

namespace injectx::stdext::tests {

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

template<typename T, size_t S>
struct Bar {
  size_t size() {
    return S;
  }
};

TEMPLATE_TEST_CASE_SIG(
    "transformable-value",
    "",
    ((typename T, bool E), T, E),
    (int, false),
    (Some, false),
    (void, true)) {
  using Expected = expected<T, std::string_view>;
  using M = Monadics<T, std::conditional_t<std::is_void_v<T>, int, T>>;
  using Value = typename M::Value;
  using LValueRef = typename M::LValueRef;
  using RValueRef = typename M::RValueRef;
  using ConstLValueRef = typename M::ConstLValueRef;
  using ConstRValueRef = typename M::ConstRValueRef;

  STATIC_REQUIRE(transformable<Expected &, Value>);
  STATIC_REQUIRE(transformable<Expected &, LValueRef>);
  STATIC_REQUIRE(transformable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(transformable<Expected &, RValueRef> == E);
  STATIC_REQUIRE(transformable<Expected &, ConstRValueRef> == E);

  STATIC_REQUIRE(transformable<const Expected &, Value>);
  STATIC_REQUIRE(transformable<const Expected &, LValueRef> == E);
  STATIC_REQUIRE(transformable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(transformable<const Expected &, RValueRef> == E);
  STATIC_REQUIRE(transformable<const Expected &, ConstRValueRef> == E);

  STATIC_REQUIRE(transformable<Expected &&, Value>);
  STATIC_REQUIRE(transformable<Expected &&, LValueRef> == E);
  STATIC_REQUIRE(transformable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(transformable<Expected &&, RValueRef>);
  STATIC_REQUIRE(transformable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(transformable<const Expected &&, Value>);
  STATIC_REQUIRE(transformable<const Expected &&, LValueRef> == E);
  STATIC_REQUIRE(transformable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(transformable<const Expected &&, RValueRef> == E);
  STATIC_REQUIRE(transformable<const Expected &&, ConstRValueRef>);
}

TEST_CASE("transform-value") {
  using Expected = expected<int, std::string_view>;

  auto getResult = [](bool initWithError) {
    auto r1 = initWithError ? Expected{unexpected{"foo"}} : Expected{10};

    auto r2 = r1 | transform([](auto &v) {
                v = 5;
                return 20;
              });
    auto r3 = std::as_const(r1) | transform([](const auto &v) {
                return v + 10;
              })
            | transform([](auto &&v) {
                return v + 1;
              });
    auto r4 = r3 | transform([](auto &) {
                return;
              });

    return std::tuple{
        std::move(r1), std::move(r2), std::move(r3), std::move(r4)};
  };

  SECTION("init with value") {
    constexpr auto r = getResult(false);
    STATIC_REQUIRE(std::get<0>(r).value() == 5);
    STATIC_REQUIRE(std::get<1>(r).value() == 20);
    STATIC_REQUIRE(std::get<2>(r).value() == 16);
    STATIC_REQUIRE(std::get<3>(r).has_value());
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(r)>::value_type>);
  }

  SECTION("init with error") {
    constexpr auto r1 = getResult(true);
    STATIC_REQUIRE(std::get<0>(r1).has_value() == false);
    STATIC_REQUIRE(std::get<1>(r1).has_value() == false);
    STATIC_REQUIRE(std::get<2>(r1).has_value() == false);
    STATIC_REQUIRE(std::get<3>(r1).has_value() == false);
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(r1)>::value_type>);
  }
}

}  // namespace injectx::stdext::tests
