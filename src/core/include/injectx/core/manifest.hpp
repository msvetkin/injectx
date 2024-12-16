// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/core/dependency_info.hpp"
#include "injectx/core/setup_traits.hpp"
// #include "injectx/stdext/empty.hpp"
#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"
#include "injectx/stdext/ranges/aliases.hpp"
#include "injectx/stdext/ranges/views/enumerate.hpp"
#include "injectx/stdext/static_format.hpp"
#include "injectx/stdext/type_name.hpp"
// #include "injectx/stdext/ranges/algorithm/binary_find.hpp"

#include <boost/pfr.hpp>
#include <fmt/core.h>
#include <gsl/span>

#include <algorithm>
#include <array>
#include <string_view>
#include <type_traits>
#include <utility>

namespace injectx::core {

namespace details::_manifest {

template<typename T>
[[nodiscard]] consteval std::string_view moduleNamespace() noexcept {
  constexpr std::string_view func = stdext::type_name<T>();

  constexpr std::string_view prefix = "modules::";
  constexpr std::string_view suffix = "::";

  constexpr auto index = func.find(prefix);
  if constexpr (index == std::string_view::npos) {
    return std::string_view{""};
  } else {
    constexpr std::string_view begin = func.substr(index).substr(prefix.size());
    return begin.substr(0, begin.find(suffix));
  }
}

template<std::size_t Size>
[[nodiscard]] consteval auto replaceColumnsWithDash(
    std::string_view str) noexcept {
  std::array<char, Size + 1> name{0};

  for (const auto [i, c] : str | stdext::rv::enumerate) {
    name[i] = c == ':' ? '-' : c;
  }

  return name;
}

template<typename T>
[[nodiscard]] consteval auto moduleName() noexcept {
  constexpr auto ns = moduleNamespace<T>();
  if constexpr (ns.empty()) {
    return std::array<char, 1>{0};
  } else {
    return replaceColumnsWithDash<ns.size()>(ns);
  }
}

template<typename T>
constexpr auto collectDependencies() {
  return []<std::size_t... Idx>(std::index_sequence<Idx...>) {
    if constexpr (sizeof...(Idx)) {
      // std::sort(provides.begin(), provides.end());
      return std::array{DependencyInfo{
          .type =
              stdext::type_name<typename boost::pfr::tuple_element_t<Idx, T>>(),
          .name = boost::pfr::get_name<Idx, T>()}...};
    } else {
      return std::array<DependencyInfo, 0>{};
    }
  }(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
}

template<typename Provides, typename Requires>
inline constexpr auto nameFrom = moduleName<std::conditional_t<
    std::same_as<Provides, std::monostate>,
    Requires,
    Provides>>();

template<typename T>
inline constexpr auto collectDependenciesFrom = collectDependencies<T>();

struct Manifest {
  std::string_view name;
  gsl::span<const DependencyInfo> dependencies;
  gsl::span<const DependencyInfo> provides;
};

template<typename Provides, typename Requires>
inline constexpr Manifest manifestFor = {
    .name = nameFrom<Provides, Requires>.data(),
    .dependencies = collectDependenciesFrom<Requires>,
    .provides = collectDependenciesFrom<Provides>,
};

struct CircularError {
  std::size_t dependency;
};

[[nodiscard]] consteval stdext::expected<void, CircularError>
dontDependOnItself(const auto &manifest) noexcept {
  if (manifest.dependencies.empty()) {
    return {};
  }

  for (const auto [index, dep] :
       manifest.dependencies | stdext::rv::enumerate) {
    // const auto it = stdext::ranges::binary_find(manifest.provides, dep);
    if (std::binary_search(
            manifest.provides.begin(), manifest.provides.end(), dep)) {
      return stdext::unexpected{CircularError{.dependency = index}};
    }
  }

  return {};
}

template<typename STraits>
[[nodiscard]] consteval stdext::expected<const Manifest *, std::string_view>
make() noexcept {
  constexpr auto manifest =
      &manifestFor<typename STraits::Provides, typename STraits::Requires>;

  if constexpr (manifest->name.empty()) {
    return stdext::unexpected{std::string_view{"invalid namespace"}};
  } else {
    constexpr auto circularDeps = dontDependOnItself(*manifest);
    if constexpr (!circularDeps.has_value()) {
      constexpr auto dep =
          manifest->dependencies[circularDeps.error().dependency].name;
      return stdext::unexpected{stdext::static_format<
          "Component '{}' provides and depends on '{}'",
          stdext::static_string<manifest->name.size()>{manifest->name},
          stdext::static_string<dep.size()>{dep}>()};
    }

    return manifest;
  }
}

}  // namespace details::_manifest

class Manifest {
 public:
  constexpr Manifest(const details::_manifest::Manifest *m) noexcept
      : m_(m) {
  }

  [[nodiscard]] constexpr std::string_view name() const noexcept {
    return m_->name;
  }

  [[nodiscard]] constexpr gsl::span<const DependencyInfo> dependencies()
      const noexcept {
    return m_->dependencies;
  }

  [[nodiscard]] constexpr gsl::span<const DependencyInfo> provides()
      const noexcept {
    return m_->provides;
  }

 private:
  const details::_manifest::Manifest *m_{nullptr};
};

template<auto setup>
[[nodiscard]] consteval auto makeManifest() noexcept {
  return details::_manifest::make<SetupTraits<setup>>() | stdext::transform([](auto m) {
           return Manifest{m};
         });
}

// template<auto setup>
// [[nodiscard]] consteval auto makeManifest() noexcept {
  // return makeManifest<SetupTraits<Setup>>>();
// }

}  // namespace injectx::core
