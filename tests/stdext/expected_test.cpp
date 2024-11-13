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

namespace injectx::stdext::tests {

namespace {

struct Some {
  // clang does not see usage of it in constexpr context
  [[maybe_unused]] friend constexpr std::strong_ordering operator<=>(
      const Some &, const Some &) = default;
};

}  // namespace

TEMPLATE_TEST_CASE_SIG(
    "type-traits",
    "",
    ((typename T, typename E, bool DI), T, E, DI),
    (void, int, true),
    (int, bool, false),
    (Some, std::string_view, false)) {
  using Expected = expected<T, E>;
  STATIC_REQUIRE(std::same_as<typename Expected::value_type, T>);
  STATIC_REQUIRE(std::same_as<typename Expected::error_type, E>);
  STATIC_REQUIRE(
      std::same_as<typename Expected::unexpected_type, unexpected<E>>);
  STATIC_REQUIRE(std::default_initializable<Expected> == DI);
  STATIC_REQUIRE(std::copy_constructible<Expected>);
  STATIC_REQUIRE(std::move_constructible<Expected>);
  STATIC_REQUIRE(
      std::constructible_from<Expected, typename Expected::unexpected_type>);
}

TEMPLATE_TEST_CASE_SIG(
    "ctor-implicit-value",
    "",
    ((typename T, auto V), T, V),
    (void, std::in_place),
    (int, 10),
    (Some, Some{})) {
  using Expected = expected<T, std::string_view>;
  constexpr auto result = std::invoke([]() -> Expected {
    return V;
  });

  STATIC_REQUIRE(result.has_value());

  if constexpr (std::is_void_v<T>) {
    result.value();
  } else {
    STATIC_REQUIRE(result.value() == V);
  }
}

TEST_CASE("value-void-ctor-default") {
  using Expected = expected<void, std::string_view>;

  constexpr Expected result;
  STATIC_REQUIRE(result.has_value());
  STATIC_REQUIRE(std::same_as<decltype(result.value()), void>);
}

TEMPLATE_TEST_CASE("ctor-implicit-unexpected", "", void, int, Some) {
  using Expected = expected<TestType, std::string_view>;
  constexpr auto result = std::invoke([]() -> Expected {
    return unexpected{"boo"};
  });

  STATIC_REQUIRE(result.has_value() == false);
  STATIC_REQUIRE(result.error() == std::string_view{"boo"});
}

TEMPLATE_TEST_CASE_SIG(
    "ctor-copy-value",
    "",
    ((typename T, auto V), T, V),
    (void, std::in_place),
    (int, 10),
    (Some, Some{})) {
  using Expected = expected<T, std::string_view>;

  constexpr Expected result{V};
  constexpr Expected result2{result};
  STATIC_REQUIRE(result2.has_value() == true);

  if constexpr (std::is_void_v<T>) {
    result.value();
  } else {
    STATIC_REQUIRE(result.value() == V);
  }
}

TEMPLATE_TEST_CASE("ctor-copy-error", "", void, int, Some) {
  using Expected = expected<TestType, std::string_view>;

  constexpr Expected result{unexpected{"fail"}};
  constexpr Expected result2{result};

  STATIC_REQUIRE(result2.has_value() == false);
  STATIC_REQUIRE(result2.error() == std::string_view{"fail"});
}

TEMPLATE_TEST_CASE_SIG(
    "ctor-move-value",
    "",
    ((typename T, auto V), T, V),
    (void, std::in_place),
    (int, 10),
    (Some, Some{})) {
  using Expected = expected<T, std::string_view>;

  constexpr auto result = std::invoke([] {
    Expected result{V};
    Expected result2{std::move(result)};
    return result2;
  });
  STATIC_REQUIRE(result.has_value() == true);

  if constexpr (std::is_void_v<T>) {
    result.value();
  } else {
    STATIC_REQUIRE(result.value() == V);
  }
}

TEMPLATE_TEST_CASE_SIG(
    "ctor-move-error",
    "",
    ((typename T, auto V), T, V),
    (void, std::in_place),
    (int, 10),
    (Some, Some{})) {
  using Expected = expected<T, std::string_view>;

  constexpr auto result = std::invoke([] {
    Expected res{unexpected{"fail"}};
    Expected res2{std::move(res)};
    return res2;
  });

  STATIC_REQUIRE(result.has_value() == false);
  STATIC_REQUIRE(result.error() == std::string_view{"fail"});
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

TEMPLATE_TEST_CASE_SIG(
    "accessors-qualifiers",
    "",
    ((typename T, auto V), T, V),
    (void, std::in_place),
    (int, 10),
    (Some, Some{})) {
  using Expected = expected<T, std::string_view>;
  using Value = QualifiersFor<T>;
  using Error = QualifiersFor<std::string_view>;

  Expected result{V};

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
