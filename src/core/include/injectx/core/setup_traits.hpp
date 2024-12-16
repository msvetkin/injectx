#pragma once

#include "injectx/core/setup_concepts.hpp"
#include "injectx/stdext/function_traits.hpp"

#include <type_traits>
#include <utility>

namespace injectx::core {

template<auto setup>
class SetupTraits {
  using Signature = decltype(setup);
  using Traits = stdext::function_traits<Signature>;

 public:
  using Function = Signature;
  using Result = typename Traits::return_type;
  using Provides = std::conditional_t<
      std::is_void_v<typename Result::value_type>,
      std::monostate,
      typename Result::value_type>;
  using Requires = typename Traits::template arg_or<0, std::monostate>;
};

}  // namespace injectx::core
