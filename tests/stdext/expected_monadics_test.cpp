// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <any>

namespace {

struct LookLikeExpected {
  using error_type = std::string_view;
};

template<typename T, typename R = T>
struct Monadics {
  using Type = T;
  using Return = R;

  using Value = decltype([](T v) -> R {
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
  using Type = void;
  using Return = R;

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

class Some {
 public:
  Some(int v)
      : value_(v) {
  }

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

}  // namespace

namespace injectx::stdext::tests {

template<typename T>
struct Transformable {
  using Expected = expected<T, std::string_view>;
  using Return = std::conditional_t<std::is_void_v<T>, int, T>;
  using M = Monadics<T, Return>;

  template<typename Expected, typename F>
  static inline constexpr bool invocable = transformable<Expected, F>;
};

template<typename T>
struct AndThenable {
  using Expected = expected<T, std::string_view>;
  using Return = typename Expected::template rebind<std::any>;
  using M = Monadics<T, Return>;

  template<typename Expected, typename F>
  static inline constexpr bool invocable = and_thenable<Expected, F>;
};

TEMPLATE_PRODUCT_TEST_CASE(
    "concept", "", (Transformable, AndThenable), (void, int, Some)) {
  using C = TestType;
  using Expected = typename TestType::Expected;
  using M = typename TestType::M;
  using Value = typename M::Value;
  using LValueRef = typename M::LValueRef;
  using RValueRef = typename M::RValueRef;
  using ConstLValueRef = typename M::ConstLValueRef;
  using ConstRValueRef = typename M::ConstRValueRef;

  constexpr auto result = std::is_void_v<typename Expected::value_type>;

  STATIC_REQUIRE(C::template invocable<Expected &, Value>);
  STATIC_REQUIRE(C::template invocable<Expected &, LValueRef>);
  STATIC_REQUIRE(C::template invocable<Expected &, ConstLValueRef>);
  STATIC_REQUIRE(C::template invocable<Expected &, RValueRef> == result);
  STATIC_REQUIRE(C::template invocable<Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(C::template invocable<const Expected &, Value>);
  STATIC_REQUIRE(C::template invocable<const Expected &, LValueRef> == result);
  STATIC_REQUIRE(C::template invocable<const Expected &, ConstLValueRef>);
  STATIC_REQUIRE(C::template invocable<const Expected &, RValueRef> == result);
  STATIC_REQUIRE(
      C::template invocable<const Expected &, ConstRValueRef> == result);

  STATIC_REQUIRE(C::template invocable<Expected &&, Value>);
  STATIC_REQUIRE(C::template invocable<Expected &&, LValueRef> == result);
  STATIC_REQUIRE(C::template invocable<Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(C::template invocable<Expected &&, RValueRef>);
  STATIC_REQUIRE(C::template invocable<Expected &&, ConstRValueRef>);

  STATIC_REQUIRE(C::template invocable<const Expected &&, Value>);
  STATIC_REQUIRE(C::template invocable<const Expected &&, LValueRef> == result);
  STATIC_REQUIRE(C::template invocable<const Expected &&, ConstLValueRef>);
  STATIC_REQUIRE(C::template invocable<const Expected &&, RValueRef> == result);
  STATIC_REQUIRE(C::template invocable<const Expected &&, ConstRValueRef>);

  if constexpr (is_expected<typename TestType::Return>) {
    STATIC_REQUIRE(
        C::template invocable<Expected &, typename M::LikeExpected> == false);
  }

  if constexpr (std::is_void_v<typename Expected::value_type>) {
    using F = decltype([](auto) -> typename M::Return {
      return 1;
    });
    STATIC_REQUIRE(C::template invocable<Expected &, F> == false);
  } else {
    using F = decltype([]() -> typename M::Return {
      return 1;
    });
    STATIC_REQUIRE(C::template invocable<Expected &, F> == false);
  }
}

template<typename T>
constexpr auto to_any(T &v) {
  struct Any {
    T value;
  };

  ++v;
  return expected<Any, std::string_view>{Any{v + 10}};
}

struct Transform {
  using Operation = decltype(transform);
  using ReturnValue = int;
  using ReturnVoid = void;
};

struct AndThen {
  using Operation = decltype(and_then);
  using ReturnValue = expected<int, std::string_view>;
  using ReturnVoid = expected<void, std::string_view>;
};

template<typename T>
constexpr auto to_void(int) -> T {
  if constexpr (is_expected<T>) {
    return {};
  } else {
    return;
  }
}

TEMPLATE_TEST_CASE("pipeline", "", Transform, AndThen) {
  auto getResults = [](auto init) {
    typename TestType::Operation op;
    expected<int, std::string_view> r1{std::move(init)};

    auto r2 = r1 | op([](auto &v) -> typename TestType::ReturnValue {
                v = 5;
                return 20;
              });
    auto r3 = std::as_const(r1)
            | op([](const auto &v) -> typename TestType::ReturnValue {
                return v + 10;
              })
            | op([](auto &&v) -> typename TestType::ReturnValue {
                return v + 1;
              });
    auto r4 = r3 | op(to_void<typename TestType::ReturnVoid>)
            | op([]() -> typename TestType::ReturnValue {
                return 1;
              })
            | op(to_void<typename TestType::ReturnVoid>);

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

}  // namespace injectx::stdext::tests
