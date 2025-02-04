#pragma once

#include "injectx/stdext/static_format.hpp"
#include "injectx/stdext/static_string.hpp"

namespace injectx::stdext {

// #if !defined(__clang__) \
    // && (defined(_MSC_VER) || (defined(__GNUC__) && __GNUC__ <= 13))
//

#if !defined(__clang__) && (defined(__GNUC__) && __GNUC__ <= 13)

template<bool C, injectx::stdext::static_string msg>
struct _static_assert {
  [[nodiscard]] static consteval bool verify() noexcept {
    return true;
  }
};

template<injectx::stdext::static_string msg>
struct _static_assert<false, msg> {
  static consteval bool verify() = delete;
};

template<bool C, injectx::stdext::static_string msg>
concept static_verify = _static_assert<C, msg>::verify();

#else

template<bool C, static_string msg>
concept _static_assert = C;

template<bool C, static_string msg>
concept static_verify = _static_assert<C, msg>;

#endif

}  // namespace injectx::stdext

#define INJECTX_STDEXT_STATIC_ASSERT(cond, fmt, ...)                 \
  [&]<bool Condition>() consteval {                                  \
    if constexpr (!Condition) {                                      \
      constexpr auto msg =                                           \
          injectx::stdext::static_format2<fmt, __VA_ARGS__>();       \
      static_assert(injectx::stdext::static_verify<Condition, msg>); \
    }                                                                \
  }.template operator()<cond>()
