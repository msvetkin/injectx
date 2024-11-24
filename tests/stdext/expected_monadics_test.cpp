// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expected.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <any>

namespace {

namespace details {

template<typename T>
struct InnerType {
  using type = T;
};

template<typename T>
struct InnerType<injectx::stdext::unexpected<T>> {
  using type = T;
};

}  // namespace details

template<typename T>
using GetInnerType = typename details::InnerType<T>::type;

struct LookLikeExpected {
  using error_type = std::string_view;
};

template<typename T, typename R = T>
struct Operations {
  using Type = T;
  using InnerType = GetInnerType<T>;
  using Return = R;

  using Value = decltype([](InnerType v) -> R {
    return T{v};
  });

  using LValueRef = decltype([](InnerType &v) -> R {
    return T{v};
  });

  using ConstLValueRef = decltype([](const InnerType &v) -> R {
    return T{v};
  });

  using RValueRef = decltype([](InnerType &&v) -> R {
    return T{v};
  });

  using ConstRValueRef = decltype([](const InnerType &&v) -> R {
    return T{v};
  });

  using ReturnLikeExpected = decltype([](InnerType) {
    return LookLikeExpected{};
  });
};

template<typename R>
struct Operations<void, R> {
  using InnerType = void;
  using Return = R;

  using Value = decltype([]() -> R {
    return 1;
  });
  using LValueRef = Value;
  using ConstLValueRef = Value;
  using RValueRef = Value;
  using ConstRValueRef = Value;
  using ReturnLikeExpected = decltype([]() {
    return LookLikeExpected{};
  });
};

class Some {
 public:
  constexpr Some(int v)
      : value_(v) {
  }

  constexpr Some &operator++() {
    ++value_;
    return *this;
  }

  constexpr Some &operator+(int value) {
    value_ += value;
    return *this;
  }

  constexpr operator int() const {
    return value_;
  }

 private:
  int value_{};
};

}  // namespace

namespace injectx::stdext::tests {

TEMPLATE_TEST_CASE("and-then-concept", "", void, int, Some) {
  using Expected = expected<TestType, std::string_view>;
  using Ops =
      Operations<TestType, typename Expected::template rebind<std::any>>;
  using Value = typename Ops::Value;
  using LValueRef = typename Ops::LValueRef;
  using RValueRef = typename Ops::RValueRef;
  using ConstLValueRef = typename Ops::ConstLValueRef;
  using ConstRValueRef = typename Ops::ConstRValueRef;

  constexpr auto result = std::is_void_v<typename Expected::value_type>;

  STATIC_REQUIRE(and_then.invocable<Expected &, Value>);
  STATIC_REQUIRE(and_then.invocable<Expected &, LValueRef>);
  STATIC_REQUIRE(and_then.invocable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(and_then.invocable<Expected &, RValueRef> == result);
  STATIC_REQUIRE(and_then.invocable<Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(and_then.invocable<const Expected &, Value>);
  STATIC_REQUIRE(and_then.invocable<const Expected &, LValueRef> == result);
  STATIC_REQUIRE(and_then.invocable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(and_then.invocable<const Expected &, RValueRef> == result);
  STATIC_REQUIRE(
      and_then.invocable<const Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(and_then.invocable<Expected &&, Value>);
  STATIC_REQUIRE(and_then.invocable<Expected &&, LValueRef> == result);
  STATIC_REQUIRE(and_then.invocable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(and_then.invocable<Expected &&, RValueRef>);
  STATIC_REQUIRE(and_then.invocable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(and_then.invocable<const Expected &&, Value>);
  STATIC_REQUIRE(and_then.invocable<const Expected &&, LValueRef> == result);
  STATIC_REQUIRE(and_then.invocable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(and_then.invocable<const Expected &&, RValueRef> == result);
  STATIC_REQUIRE(and_then.invocable<const Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(
      and_then.invocable<Expected &, typename Ops::ReturnLikeExpected>
      == false);

  if constexpr (std::is_void_v<typename Expected::value_type>) {
    using F = decltype([](auto) -> typename Ops::Return {
      return 1;
    });
    STATIC_REQUIRE(and_then.invocable<Expected &, F> == false);
  } else {
    using F = decltype([]() -> typename Ops::Return {
      return 1;
    });
    STATIC_REQUIRE(and_then.invocable<Expected &, F> == false);
  }
}

TEST_CASE("and-then") {
  auto getResults = [](auto init) {
    using Expected = expected<int, std::string_view>;
    Expected r1{std::move(init)};

    auto r2 = r1 | and_then([](int &v) -> Expected {
                v = 5;
                return 20;
              });
    auto r3 = std::as_const(r1) | and_then([](const auto &v) -> Expected {
                return v + 10;
              })
            | and_then([](auto &&v) -> Expected {
                return v + 1;
              });
    auto r4 = r3 | and_then([](auto) -> expected<void, std::string_view> {
                return {};
              })
            | and_then([]() -> Expected {
                return 1;
              })
            | and_then([](auto) -> expected<void, std::string_view> {
                return {};
              });

    return std::tuple{
        std::move(r1), std::move(r2), std::move(r3), std::move(r4)};
  };

  SECTION("with value") {
    constexpr auto rs = getResults(10);
    STATIC_REQUIRE(std::get<0>(rs).value() == 5);
    STATIC_REQUIRE(std::get<1>(rs).value() == 20);
    STATIC_REQUIRE(std::get<2>(rs).value() == 16);
    STATIC_REQUIRE(std::get<3>(rs).has_value());
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(rs)>::value_type>);
  }

  SECTION("with error") {
    constexpr std::string_view msg{"foo"};
    constexpr auto rs = getResults(unexpected{msg});
    STATIC_REQUIRE(std::get<0>(rs).error() == msg);
    STATIC_REQUIRE(std::get<1>(rs).error() == msg);
    STATIC_REQUIRE(std::get<2>(rs).error() == msg);
    STATIC_REQUIRE(std::get<3>(rs).error() == msg);
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(rs)>::value_type>);
  }
}

TEMPLATE_TEST_CASE("transform-concept", "", void, int, Some) {
  using Expected = expected<TestType, std::string_view>;
  using Return = std::conditional_t<std::is_void_v<TestType>, int, TestType>;
  using Op = Operations<TestType, Return>;
  using Value = typename Op::Value;
  using LValueRef = typename Op::LValueRef;
  using RValueRef = typename Op::RValueRef;
  using ConstLValueRef = typename Op::ConstLValueRef;
  using ConstRValueRef = typename Op::ConstRValueRef;

  constexpr auto result = std::is_void_v<typename Expected::value_type>;

  STATIC_REQUIRE(transform.invocable<Expected &, Value>);
  STATIC_REQUIRE(transform.invocable<Expected &, LValueRef>);
  STATIC_REQUIRE(transform.invocable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(transform.invocable<Expected &, RValueRef> == result);
  STATIC_REQUIRE(transform.invocable<Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(transform.invocable<const Expected &, Value>);
  STATIC_REQUIRE(transform.invocable<const Expected &, LValueRef> == result);
  STATIC_REQUIRE(transform.invocable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(transform.invocable<const Expected &, RValueRef> == result);
  STATIC_REQUIRE(
      transform.invocable<const Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(transform.invocable<Expected &&, Value>);
  STATIC_REQUIRE(transform.invocable<Expected &&, LValueRef> == result);
  STATIC_REQUIRE(transform.invocable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(transform.invocable<Expected &&, RValueRef>);
  STATIC_REQUIRE(transform.invocable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(transform.invocable<const Expected &&, Value>);
  STATIC_REQUIRE(transform.invocable<const Expected &&, LValueRef> == result);
  STATIC_REQUIRE(transform.invocable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(transform.invocable<const Expected &&, RValueRef> == result);
  STATIC_REQUIRE(transform.invocable<const Expected &&, ConstRValueRef>);

  if constexpr (std::is_void_v<typename Expected::value_type>) {
    using F = decltype([](auto) -> typename Op::Return {
      return 1;
    });
    STATIC_REQUIRE(transform.invocable<Expected &, F> == false);
  } else {
    using F = decltype([]() -> typename Op::Return {
      return 1;
    });
    STATIC_REQUIRE(transform.invocable<Expected &, F> == false);
  }
}

constexpr auto to_void(int) {
  return;
}

TEST_CASE("transform") {
  auto getResults = [](auto init) {
    expected<int, std::string_view> r0{std::move(init)};

    auto r1 = r0 | transform([](int &v) {
                v = 5;
                return 20;
              });
    auto r2 = std::as_const(r0) | transform([](const auto &v) {
                return v + 10;
              })
            | transform([](auto &&v) {
                return v + 1;
              });
    auto r3 = r2 | transform(to_void) | transform([]() {
                return 1;
              })
            | transform(to_void);

    return std::tuple{
        std::move(r0), std::move(r1), std::move(r2), std::move(r3)};
  };

  SECTION("with value") {
    constexpr auto rs = getResults(10);
    STATIC_REQUIRE(std::get<0>(rs).value() == 5);
    STATIC_REQUIRE(std::get<1>(rs).value() == 20);
    STATIC_REQUIRE(std::get<2>(rs).value() == 16);
    STATIC_REQUIRE(std::get<3>(rs).has_value());
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(rs)>::value_type>);
  }

  SECTION("with error") {
    constexpr std::string_view msg{"foo"};
    constexpr auto rs = getResults(unexpected{msg});
    STATIC_REQUIRE(std::get<0>(rs).error() == msg);
    STATIC_REQUIRE(std::get<1>(rs).error() == msg);
    STATIC_REQUIRE(std::get<2>(rs).error() == msg);
    STATIC_REQUIRE(std::get<3>(rs).error() == msg);
    STATIC_REQUIRE(std::is_void_v<
                   typename std::tuple_element_t<3, decltype(rs)>::value_type>);
  }
}

TEMPLATE_TEST_CASE("or-else-concept", "", int, Some) {
  using Expected = expected<std::string_view, TestType>;
  using Op = Operations<
      unexpected<TestType>, typename Expected::template rebind_error<std::any>>;
  using Value = typename Op::Value;
  using LValueRef = typename Op::LValueRef;
  using RValueRef = typename Op::RValueRef;
  using ConstLValueRef = typename Op::ConstLValueRef;
  using ConstRValueRef = typename Op::ConstRValueRef;

  STATIC_REQUIRE(or_else.invocable<Expected &, Value>);
  STATIC_REQUIRE(or_else.invocable<Expected &, LValueRef>);
  STATIC_REQUIRE(or_else.invocable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(or_else.invocable<Expected &, RValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<Expected &, ConstRValueRef> == false);

  STATIC_REQUIRE(or_else.invocable<const Expected &, Value>);
  STATIC_REQUIRE(or_else.invocable<const Expected &, LValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(or_else.invocable<const Expected &, RValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<const Expected &, ConstRValueRef> == false);

  STATIC_REQUIRE(or_else.invocable<Expected &&, Value>);
  STATIC_REQUIRE(or_else.invocable<Expected &&, LValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(or_else.invocable<Expected &&, RValueRef>);
  STATIC_REQUIRE(or_else.invocable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(or_else.invocable<const Expected &&, Value>);
  STATIC_REQUIRE(or_else.invocable<const Expected &&, LValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(or_else.invocable<const Expected &&, RValueRef> == false);
  STATIC_REQUIRE(or_else.invocable<const Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(
      or_else.invocable<Expected &, typename Op::ReturnLikeExpected> == false);

  using F = decltype([]() -> Expected {
    return unexpected{TestType{1}};
  });
  STATIC_REQUIRE(or_else.invocable<Expected &, F> == false);
}

TEST_CASE("or-else") {
  auto getResults = [](auto init) {
    using Expected = expected<std::string_view, int>;
    Expected r1{std::move(init)};

    auto r2 = r1 | or_else([](int &v) -> expected<std::string_view, Some> {
                v = 5;
                return unexpected{20};
              });
    auto r3 = std::as_const(r1) | or_else([](const auto &v) -> Expected {
                return unexpected{v + 10};
              })
            | or_else([](auto &&v) -> Expected {
                return unexpected{v + 1};
              });

    return std::tuple{std::move(r1), std::move(r2), std::move(r3)};
  };

  SECTION("with value") {
    constexpr std::string_view msg{"foo"};
    constexpr auto rs = getResults(msg);
    STATIC_REQUIRE(std::get<0>(rs).value() == msg);
    STATIC_REQUIRE(std::get<1>(rs).value() == msg);
    STATIC_REQUIRE(std::same_as<
                   std::remove_cvref_t<std::tuple_element_t<1, decltype(rs)>>,
                   expected<std::string_view, Some>>);

    STATIC_REQUIRE(std::get<2>(rs).value() == msg);
  }

  SECTION("with error") {
    constexpr auto rs = getResults(unexpected{10});
    STATIC_REQUIRE(std::get<0>(rs).error() == 5);
    STATIC_REQUIRE(std::get<1>(rs).error() == 20);
    STATIC_REQUIRE(std::same_as<
                   std::remove_cvref_t<std::tuple_element_t<1, decltype(rs)>>,
                   expected<std::string_view, Some>>);
    STATIC_REQUIRE(std::get<2>(rs).error() == 16);
  }
}

}  // namespace injectx::stdext::tests
