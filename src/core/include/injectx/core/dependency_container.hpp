// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expected.hpp"
#include "injectx/stdext/monadics.hpp"
#include "injectx/stdext/type_name.hpp"

#include <boost/pfr.hpp>
#include <fmt/ranges.h>

#include <any>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace injectx::core {

namespace details::_dependency_container {

using Storage = std::unordered_map<std::string_view, std::any>;

template<std::size_t Idx, typename T>
using Field = boost::pfr::tuple_element_t<Idx, T>;

template<typename T>
using GetExpected = stdext::expected<T, std::string>;

using InsertExpected = stdext::expected<void, std::string>;

template<typename T>
GetExpected<T> get(const Storage &storage, std::string_view name) noexcept {
  const auto it = storage.find(name);
  if (it == storage.end()) {
    return stdext::unexpected{fmt::format(
        "'{} {}' has not been provided", stdext::type_name<T>(), name)};
  }

  if (const auto is_callable = std::any_cast<std::function<T()>>(&it->second);
      is_callable) {
    return std::invoke(*is_callable);
  } else if (const auto t = std::any_cast<T>(&it->second); t) {
    return *t;
  }

  return stdext::unexpected{
      fmt::format("'{} {}' has different type", stdext::type_name<T>(), name)};
}

template<typename T, std::size_t... Idx>
GetExpected<T> get(
    const Storage &storage, std::index_sequence<Idx...>) noexcept {
  std::vector<std::string> errors;

  [[maybe_unused]] std::tuple fields = {std::invoke([&] {
    constexpr auto name = boost::pfr::get_name<Idx, T>();
    auto field = get<Field<Idx, T>>(storage, name);

    if (!field.has_value()) {
      errors.push_back(field.error());
    }

    return field;
  })...};

  if (!errors.empty()) {
    return stdext::unexpected{fmt::format(
        "Dependenc{} {}", errors.size() == 1 ? "y" : "ies",
        fmt::join(errors, ", "))};
  }

  return T{std::get<Idx>(std::move(fields)).value()...};
}

template<typename T>
InsertExpected insert(
    Storage &storage, std::string_view name, const T &value) noexcept {
  const auto &[_, res] = storage.try_emplace(name, value);
  if (!res) {
    return stdext::unexpected{fmt::format(
        "Dependency '{} {}' has been already provided", stdext::type_name<T>(),
        name)};
  }

  return {};
}

template<typename T, std::size_t... Idx>
InsertExpected insert(
    Storage &storage, const T &value, std::index_sequence<Idx...>) noexcept {
  using Expected = stdext::expected<std::string_view, std::string>;

  std::vector<Expected> results;
  const auto inserted =
      (std::invoke([&] {
         constexpr auto name = boost::pfr::get_name<Idx, T>();

         auto res = insert(storage, name, boost::pfr::get<Idx>(value))
                  | stdext::transform([name] {
                      return name;
                    });

         results.push_back(std::move(res));
         return results.back().has_value();
       })
       && ...);

  if (!inserted) {
    for (const auto &result : results) {
      if (result.has_value()) {
        storage.erase(result.value());
      }
    }

    return stdext::unexpected{results.back().error()};
  }

  return {};
}

}  // namespace details::_dependency_container

class DependencyContainer {
 public:
  template<typename Provides>
  [[nodiscard]] auto provide(const Provides &provides) noexcept {
    constexpr auto fieldsCount = boost::pfr::tuple_size_v<Provides>;
    return details::_dependency_container::insert(
        storage_, provides, std::make_index_sequence<fieldsCount>{});
  }

  template<typename Requires>
  [[nodiscard]] auto resolve() const noexcept {
    constexpr auto fieldsCount = boost::pfr::tuple_size_v<Requires>;
    return details::_dependency_container::get<Requires>(
        storage_, std::make_index_sequence<fieldsCount>{});
  }

 private:
  details::_dependency_container::Storage storage_;
};

}  // namespace injectx::core
