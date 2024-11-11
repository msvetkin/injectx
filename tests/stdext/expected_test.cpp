// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/expected.hpp"

#include <catch2/catch_template_test_macros.hpp>

#include <concepts>
#include <functional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#define ExpectedTestedValueTypes void, int, Some
#define TYPED_TEST_CASE(name) \
  TEMPLATE_TEST_CASE(name, "", ExpectedTestedValueTypes)

namespace {

struct Some {
  // clang does not see usage of it in checkValue
  [[maybe_unused]] friend constexpr std::strong_ordering operator<=>(
      const Some &, const Some &) = default;
};

using Types = std::tuple<ExpectedTestedValueTypes>;
constexpr auto Values = std::make_tuple(std::in_place, 10, Some{});
static_assert(
    std::tuple_size_v<Types> == std::tuple_size_v<decltype(Values)>,
    "Types and Values should match");

template<typename T>
[[nodiscard]] constexpr auto getValue() noexcept {
  if constexpr (std::is_void_v<T>) {
    return std::get<std::in_place_t>(Values);
  } else {
    return std::get<T>(Values);
  }
}

template<typename T, injectx::stdext::is_expected Expected>
constexpr bool checkValue(const Expected &expected) noexcept {
  if constexpr (std::is_void_v<T>) {
    return expected.has_value() && std::is_void_v<decltype(expected.value())>;
  } else {
    return expected.value() == getValue<T>();
  }
}

}  // namespace

namespace injectx::stdext::tests {

TYPED_TEST_CASE("type-traits") {
  using Expected = expected<TestType, std::string_view>;
  STATIC_REQUIRE(std::same_as<typename Expected::value_type, TestType>);
  STATIC_REQUIRE(std::same_as<typename Expected::error_type, std::string_view>);
  STATIC_REQUIRE(
      std::same_as<
          typename Expected::unexpected_type, unexpected<std::string_view>>);
  STATIC_REQUIRE(
      std::default_initializable<Expected>
      == std::is_void_v<typename Expected::value_type>);
  STATIC_REQUIRE(std::copy_constructible<Expected>);
  STATIC_REQUIRE(std::move_constructible<Expected>);
  STATIC_REQUIRE(
      std::constructible_from<Expected, typename Expected::unexpected_type>);
}

TYPED_TEST_CASE("ctor-implicit") {
  using Expected = expected<TestType, std::string_view>;

  constexpr auto result = std::invoke([]() -> Expected {
    return getValue<TestType>();
  });

  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(checkValue<TestType>(result));
}

TEST_CASE("value-void-ctor-default") {
  using Expected = expected<void, std::string_view>;

  constexpr Expected result;
  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(std::same_as<decltype(result.value()), void>);
}

TYPED_TEST_CASE("ctor-implicit-unexpected") {
  using Expected = expected<TestType, std::string_view>;

  constexpr auto result = []() -> Expected {
    return unexpected{"boo"};
  }();
  STATIC_REQUIRE(result.has_value() == false);
  STATIC_REQUIRE(result.error() == std::string_view{"boo"});
}

TYPED_TEST_CASE("ctor-copy-value") {
  using Expected = expected<TestType, std::string_view>;

  constexpr Expected result{getValue<TestType>()};
  constexpr Expected result2{result};
  STATIC_REQUIRE(result2.has_value() == true);
  STATIC_REQUIRE(checkValue<TestType>(result2));
}

TYPED_TEST_CASE("ctor-copy-error") {
  using Expected = expected<TestType, std::string_view>;

  constexpr Expected result{unexpected{"fail"}};
  constexpr Expected result2{result};
  STATIC_REQUIRE(result2.has_value() == false);
  STATIC_REQUIRE(result2.error() == std::string_view{"fail"});
}

TYPED_TEST_CASE("ctor-move-value") {
  using Expected = expected<TestType, std::string_view>;

  constexpr auto result = []() {
    Expected result{getValue<TestType>()};
    Expected result2{std::move(result)};
    return result2;
  }();
  STATIC_REQUIRE(result.has_value() == true);
  STATIC_REQUIRE(checkValue<TestType>(result));
}

TYPED_TEST_CASE("ctor-move-error") {
  using Expected = expected<TestType, std::string_view>;

  constexpr auto res = []() {
    Expected res{unexpected{"fail"}};
    Expected res2{std::move(res)};
    return res2;
  }();
  STATIC_REQUIRE(res.has_value() == false);
  STATIC_REQUIRE(res.error() == std::string_view{"fail"});
}

template<typename T>
struct QualifiersFor {
  using pointer = T *;
  using const_pointer = const T *;
  using lvalue_reference = T &;
  using const_lvalue_reference = const T &;
  using rvalue_reference = T &&;
  using const_rvalue_reference = const T &&;
};

template<>
struct QualifiersFor<void> {
  using pointer = void;
  using const_pointer = void;
  using lvalue_reference = void;
  using const_lvalue_reference = void;
  using rvalue_reference = void;
  using const_rvalue_reference = void;
};

TYPED_TEST_CASE("accessors-qualifiers") {
  using Expected = expected<TestType, std::string_view>;
  using Value = QualifiersFor<TestType>;
  using Error = QualifiersFor<std::string_view>;

  Expected result{getValue<TestType>()};

  STATIC_REQUIRE(
      std::same_as<decltype(*result), typename Value::lvalue_reference>);
  STATIC_REQUIRE(
      std::same_as<decltype(result.value()), typename Value::lvalue_reference>);
  STATIC_REQUIRE(
      std::same_as<decltype(result.error()), typename Error::lvalue_reference>);

  STATIC_REQUIRE(std::same_as<
                 decltype(*std::as_const(result)),
                 typename Value::const_lvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::as_const(result).value()),
                 typename Value::const_lvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::as_const(result).error()),
                 typename Error::const_lvalue_reference>);

  STATIC_REQUIRE(
      std::same_as<
          decltype(*std::move(result)), typename Value::rvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::move(result).value()),
                 typename Value::rvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::move(result).error()),
                 typename Error::rvalue_reference>);

  STATIC_REQUIRE(std::same_as<
                 decltype(*std::move(std::as_const(result))),
                 typename Value::const_rvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::move(std::as_const(result)).value()),
                 typename Value::const_rvalue_reference>);
  STATIC_REQUIRE(std::same_as<
                 decltype(std::move(std::as_const(result)).error()),
                 typename Error::const_rvalue_reference>);
}

}  // namespace injectx::stdext::tests
