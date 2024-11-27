#pragma once

#include <compare>
#include <string_view>

namespace injectx::core {

struct DependencyInfo {
  std::string_view type;
  std::string_view name;

  friend constexpr std::strong_ordering operator<=>(
      const DependencyInfo &, const DependencyInfo &) = default;
};

}  // namespace injectx::core
