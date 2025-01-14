#pragma once

#include "injectx/core/setup_task.hpp"
#include "injectx/stdext/function_traits.hpp"

#include <boost/pfr/tuple_size.hpp>

#include <type_traits>

namespace injectx::core {

namespace details::_setup_concepts {

template<typename T, std::size_t Args>
concept ZeroArgsAndNoneVoid = requires {
  requires(Args == 0);
  requires !std::is_void_v<typename T::value_type>;
  typename boost::pfr::tuple_size<typename T::value_type>;
};

template<typename T, std::size_t Args>
concept ValidReturnType = requires {
  requires IsSetupTask<T>;
  requires(ZeroArgsAndNoneVoid<T, Args> || Args == 1);
};

template<typename FTraits>
concept ValidFirstArg = requires {
  requires FTraits::args_count == 1;
  requires std::is_class_v<typename FTraits::template arg<0>>;
  typename boost::pfr::tuple_size<typename FTraits::template arg<0>>;
};

template<typename FTraits>
concept ValidArguments =
    requires { requires(FTraits::args_count == 0 || ValidFirstArg<FTraits>); };

template<typename FTraits>
concept ValidSignature = requires {
  requires ValidReturnType<typename FTraits::return_type, FTraits::args_count>;
  requires ValidArguments<FTraits>;
};

template<typename T>
concept IsSetup = requires {
  requires std::is_function_v<T>;
  requires ValidSignature<stdext::function_traits<std::add_pointer_t<T>>>;
};

}  // namespace details::_setup_concepts

template<typename T>
concept IsSetupFunction =
    details::_setup_concepts::IsSetup<std::remove_pointer_t<T>>;

}  // namespace injectx::core
