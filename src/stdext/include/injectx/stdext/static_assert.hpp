#pragma once

#include "injectx/stdext/static_format.hpp"
#include "injectx/stdext/static_string.hpp"

namespace injectx::stdext {

#if !defined(__clang__) \
    && (defined(_MSC_VER) || (defined(__GNUC__) && __GNUC__ <= 13))

template<bool, static_string>
struct _static_assert;

template<static_string msg>
struct _static_assert<true, msg> {
  inline constexpr operator bool() const noexcept {
    return true;
  }
};

template<bool C, static_string msg>
inline constexpr auto static_verify = _static_assert<C, msg>{};

#else

template<bool C, static_string msg>
concept _static_assert = C;

template<bool C, static_string msg>
concept static_verify = _static_assert<C, msg>;

#endif

}  // namespace injectx::stdext

#define INJECTX_STDEXT_STATIC_ASSERT(cond, fmt, ...)                       \
  [&]<bool Condition>() consteval {                                        \
    if constexpr (!Condition) {                                            \
      constexpr auto msg =                                                 \
          injectx::stdext::static_format<fmt, __VA_ARGS__>(std::in_place); \
      static_assert(injectx::stdext::static_verify<Condition, msg>);       \
    }                                                                      \
  }.template operator()<cond>()
