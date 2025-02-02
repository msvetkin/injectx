#pragma once

#include "injectx/stdext/static_format.hpp"
#include "injectx/stdext/static_string.hpp"

namespace injectx::stdext {

#if !defined(__clang__) \
    && (defined(_MSC_VER) || (defined(__GNUC__) && __GNUC__ <= 13))

template<bool C, injectx::stdext::static_string msg>
struct panic {
  static consteval bool emit() {
    return true;
  }
};

template<injectx::stdext::static_string msg>
struct panic<false, msg> {
  static consteval bool emit() = delete;
};

template<bool C, injectx::stdext::static_string msg>
inline constexpr bool verify = panic<C, msg>::emit();

#else

template<bool C, static_string msg>
concept panic = C;

template<bool C, static_string msg>
concept verify = panic<C, msg>;

#endif

}  // namespace injectx::stdext

#define STDEXT_STATIC_ASSERT(cond, fmt, ...)                                 \
  [&]<bool Condition>() consteval {                                          \
    if constexpr (!Condition) {                                              \
      constexpr auto msg = injectx::stdext::static_format<fmt, __VA_ARGS__>; \
      static_assert(injectx::stdext::verify<Condition, msg>);                \
    }                                                                        \
  }.template operator()<cond>()
