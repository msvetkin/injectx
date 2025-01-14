// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <injectx/core/manifest.hpp>
#include <injectx/core/module.hpp>
#include <injectx/stdext/expected.hpp>
#include <injectx/stdext/monadics.hpp>
#include <injectx/stdext/ranges/aliases.hpp>
#include <injectx/stdext/ranges/views/enumerate.hpp>
#include <injectx/stdext/static_format.hpp>
#include <injectx/stdext/static_map.hpp>
#include <injectx/stdext/static_queue.hpp>
#include <tuple>

namespace injectx::core {

namespace details::_bundle {

struct ProvidesMapSizeFn {
  using Expected = stdext::expected<std::size_t, std::string_view>;

  [[nodiscard]] constexpr Expected operator()(
      const auto &manifests) const noexcept {
    std::size_t provides{};
    for (const auto &manifest : manifests) {
      if (!manifest.has_value()) {
        return stdext::unexpected{manifest.error()};
      }

      provides += manifest->provides().size();
    }

    return provides;
  }
};

inline constexpr ProvidesMapSizeFn providesMapSize{};

template<std::size_t MaxSize>
struct ProvidesMapFn {
  struct Error {
    std::size_t manifest1;
    std::size_t manifest2;
    std::size_t dependency2;
  };

  using Map = stdext::static_map<DependencyInfo, std::size_t, MaxSize>;
  using Expected = stdext::expected<Map, Error>;

  [[nodiscard]] constexpr Expected operator()(
      const auto &manifests) const noexcept {
    Map map;

    for (const auto &[mIndex, manifest] : manifests | stdext::rv::enumerate) {
      const auto provides = manifest->provides();
      for (const auto &[pIndex, provide] : provides | stdext::rv::enumerate) {
        const auto res = map.try_emplace(provide, mIndex);
        if (!res.has_value()) {
          return stdext::unexpected{Error{
              .manifest1 = res.error().has_value() ? res.error().value()->second
                                                   : mIndex,
              .manifest2 = mIndex,
              .dependency2 = pIndex,
          }};
        }
      }
    }

    return map;
  }
};

[[nodiscard]] constexpr auto buildProvidesMap(auto getManifests) noexcept {
  constexpr auto manifests = getManifests();
  constexpr auto size = providesMapSize(manifests);
  if constexpr (!size.has_value()) {
    return size;
  } else {
    constexpr auto map = ProvidesMapFn<size.value()>{}(manifests);
    using Expected =
        typename decltype(map)::template rebind_error<std::string_view>;

    if constexpr (!map.has_value()) {
      constexpr auto e = map.error();
      return Expected{stdext::unexpected{stdext::static_format<
          "Dependency '{}' provided by two modules '{}' and '{}'",
          STDEXT_AS_STATIC_STRING(
              manifests[e.manifest2]->provides()[e.dependency2].name),
          STDEXT_AS_STATIC_STRING(manifests[e.manifest1]->name()),
          STDEXT_AS_STATIC_STRING(manifests[e.manifest2]->name())>()}};
    } else {
      return Expected{map.value()};
    }
  }
}

template<std::size_t MSize>
struct GraphAndInDegreeFn {
  struct Error {
    std::size_t manifest;
    std::size_t dependency;
  };

  using Graph = std::array<std::array<std::size_t, MSize>, MSize>;
  using InDegree = std::array<std::size_t, MSize>;
  using InDegreeQueue = stdext::static_queue<std::size_t, MSize>;
  using Expected =
      stdext::expected<std::tuple<Graph, InDegree, InDegreeQueue>, Error>;

  [[nodiscard]] constexpr Expected operator()(
      const auto &manifests, const auto &providesMap) const noexcept {
    Graph graph{};
    InDegree inDegree{};
    InDegreeQueue zeroInDegreeQueue{};

    for (const auto &[dependent, manifest] :
         manifests | stdext::rv::enumerate) {
      const auto dependencies = manifest->dependencies();
      for (const auto &[depIndex, dependency] :
           dependencies | stdext::rv::enumerate) {
        const auto it = providesMap.find(dependency);
        if (it == providesMap.end()) {
          return stdext::unexpected{
              Error{.manifest = dependent, .dependency = depIndex}};
        }

        const auto [_, provider] = *it;

        graph[provider][dependent] += 1;
        inDegree[dependent]++;
      }
    }

    for (const auto &[component, degree] : inDegree | stdext::rv::enumerate) {
      if (degree == 0) {
        (void)zeroInDegreeQueue.push(component);
      }
    }

    return std::make_tuple(graph, inDegree, zeroInDegreeQueue);
  }
};

[[nodiscard]] constexpr auto buildGraphAndInDegrees(
    auto getManifests) noexcept {
  constexpr auto providesMap = buildProvidesMap(getManifests);
  if constexpr (!providesMap.has_value()) {
    return providesMap;
  } else {
    constexpr auto manifests = getManifests();
    constexpr auto parts =
        GraphAndInDegreeFn<manifests.size()>{}(manifests, providesMap.value());
    using Expected =
        typename decltype(parts)::template rebind_error<std::string_view>;

    if constexpr (!parts.has_value()) {
      constexpr auto e = parts.error();
      constexpr auto manifest = manifests[e.manifest];
      constexpr auto dependency = manifest->dependencies()[e.dependency];
      return Expected{stdext::unexpected{stdext::static_format<
          "Component '{}' could not resolve dependency: {} {}",
          STDEXT_AS_STATIC_STRING(manifest->name()),
          STDEXT_AS_STATIC_STRING(dependency.type),
          STDEXT_AS_STATIC_STRING(dependency.name)>()}};
    } else {
      return Expected{parts.value()};
    }
  }
}

template<std::size_t MSize>
struct TopologicalSortFn {
  using Order = std::array<std::size_t, MSize>;
  using Expected = stdext::expected<Order, std::string_view>;

  [[nodiscard]] constexpr Expected operator()(
      const auto &manifests,
      const auto &adjList,
      auto inDegree,
      auto zeroInDegreeQueue) noexcept {
    Order order{};

    std::size_t processedCount = 0;
    while (!zeroInDegreeQueue.empty() && processedCount < manifests.size()) {
      const auto component = zeroInDegreeQueue.front();
      if (!component.has_value()) {
        return stdext::unexpected{component.error()};
      }

      if (const auto poped = zeroInDegreeQueue.pop(); !poped.has_value()) {
        return stdext::unexpected{poped.error()};
      }

      order[processedCount] = component.value();

      std::size_t dependent{};
      for (const auto depends_on : adjList[component.value()]) {
        if (depends_on != 0) {
          inDegree[dependent]--;
          if (inDegree[dependent] == 0) {
            if (const auto pushed = zeroInDegreeQueue.push(dependent);
                !pushed.has_value()) {
              return stdext::unexpected{pushed.error()};
            }
          }
        }

        dependent++;
      }

      processedCount++;
    }

    if (processedCount != manifests.size()) {
      return stdext::unexpected{std::string_view{"circular dependencies"}};
    }

    return order;
  }

  [[nodiscard]] constexpr Expected operator()(auto getManifests) noexcept {
    constexpr auto graphAndInDegrees = buildGraphAndInDegrees(getManifests);
    if constexpr (!graphAndInDegrees.has_value()) {
      return stdext::unexpected{graphAndInDegrees.error()};
    } else {
      auto [adjList, inDegree, zeroInDegreeQueue] = graphAndInDegrees.value();
      constexpr auto manifests = getManifests();
      return this->operator()(manifests, adjList, inDegree, zeroInDegreeQueue);
    }
  }
};

template<auto... setups>
[[nodiscard]] constexpr auto topologicalSort() noexcept {
  auto getManifests = []() constexpr {
    return std::array{makeManifest<setups>()...};
  };

  return TopologicalSortFn<sizeof...(setups)>{}(getManifests);
}

struct Bundle {
  gsl::span<const Module> modules;
};

template<auto... setups>
inline constexpr auto modulesFor = std::invoke([] {
  return std::array{makeModule<setups>().value()...};
});

template<auto... setups>
inline constexpr Bundle bundleFor = {.modules = modulesFor<setups...>};

template<auto... setups>
[[nodiscard]] consteval auto make() noexcept {
  using Expected = stdext::expected<const Bundle *, std::string_view>;

  constexpr auto sorted = topologicalSort<setups...>();
  if constexpr (!sorted.has_value()) {
    return Expected{stdext::unexpected{sorted.error()}};
  } else {
    constexpr auto t = std::make_tuple(setups...);

    return std::invoke(
        [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
          return Expected{&bundleFor<std::get<sorted.value()[Idx]>(t)...>};
        },
        std::index_sequence_for<decltype(setups)...>{});
  }
}

}  // namespace details::_bundle

class Bundle {
 public:
  constexpr Bundle(const details::_bundle::Bundle *b) noexcept
      : b_(b) {
  }

  [[nodiscard]] constexpr const Module &operator[](
      std::size_t index) const noexcept {
    return b_->modules[index];
  }

  [[nodiscard]] constexpr const Module &at(std::size_t index) const noexcept {
    return b_->modules[index];
  }

  [[nodiscard]] constexpr auto size() const noexcept {
    return b_->modules.size();
  }

  [[nodiscard]] constexpr auto begin() const noexcept {
    return b_->modules.begin();
  }

  [[nodiscard]] constexpr auto end() const noexcept {
    return b_->modules.end();
  }

 private:
  const details::_bundle::Bundle *b_{nullptr};
};

template<IsSetupFunction auto... setups>
[[nodiscard]] consteval auto makeBundle() noexcept {
  return details::_bundle::make<setups...>() | stdext::transform([](auto b) {
           return Bundle{b};
         });
}

}  // namespace injectx::core
