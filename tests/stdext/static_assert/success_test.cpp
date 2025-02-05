// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#include "injectx/stdext/static_assert.hpp"
#include "injectx/stdext/static_string.hpp"
#include "injectx/stdext/type_name.hpp"

#include <catch2/catch_template_test_macros.hpp>

namespace injectx::stdext::tests {

TEMPLATE_TEST_CASE("success", "", int) {
  constexpr auto value = TestType{};
  constexpr auto type = type_name<TestType>();
  INJECTX_STDEXT_STATIC_ASSERT(
      value == TestType{}, "{}{{}} should be {}", STDEXT_AS_STATIC_STRING(type),
      value);
}

}  // namespace injectx::stdext::tests
