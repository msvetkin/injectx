// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expected.hpp"

#include <array>
#include <iterator>
#include <optional>
#include <utility>

namespace injectx::stdext {

namespace details::_static_map {

template<typename Iterator>
class [[nodiscard]] iterator_wrapper {
  using CountedIt = std::counted_iterator<Iterator>;

 public:
  constexpr iterator_wrapper() = default;

  constexpr iterator_wrapper(Iterator it, std::iter_difference_t<Iterator> count) noexcept
    : countedIt_{it, count} {
  }

  constexpr decltype(auto) operator*() const noexcept {
    return countedIt_->value();
  }

  constexpr decltype(auto) operator->() const noexcept {
    return &countedIt_->value();
  }

  constexpr iterator_wrapper& operator++() noexcept {
    ++countedIt_;
    return *this;
  }

  friend constexpr std::strong_ordering operator<=>(
      const iterator_wrapper&, const iterator_wrapper&) = default;

  friend constexpr bool operator==(
      const iterator_wrapper& it, std::default_sentinel_t) {
    return it.countedIt_ == std::default_sentinel;
  }

 private:
  CountedIt countedIt_;
};

}  // namespace details::_static_map

template<typename Key, typename Value, std::size_t MaxSize>
class static_map {
  using Optional = std::optional<std::pair<const Key, Value>>;

 public:
  using value_type = typename Optional::value_type;
  using key_type = typename value_type::first_type;
  using mapped_type = typename value_type::second_type;
  using container_type = std::array<Optional, MaxSize>;
  using iterator = details::_static_map::iterator_wrapper<typename container_type::iterator>;
  using const_iterator = details::_static_map::iterator_wrapper<typename container_type::const_iterator>;

  constexpr static_map() = default;

  [[nodiscard]] constexpr bool empty() const noexcept {
    return size_ == 0;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr std::size_t max_size() const noexcept {
    return MaxSize;
  }

  template<typename... Args>
  constexpr expected<iterator, std::optional<iterator>> try_emplace(
      const Key& key, Args&&... args) noexcept {
    if (auto it = find(key); it != end()) {
      return stdext::unexpected{std::optional{it}};
    };

    if (size_ >= MaxSize) {
      return stdext::unexpected{std::nullopt};
    }

    data_[size_++].emplace(key, Value{std::forward<Args>(args)...});
    return iterator{data_.begin() + (size_ - 1), std::ssize(*this)};
  }

  [[nodiscard]] constexpr const_iterator find(const Key& key) const noexcept {
    const auto index = find_index(key);
    if (index.has_value() == false) {
      return {};
    }

    return {data_.begin() + *index, std::ssize(*this)};
  }

  [[nodiscard]] constexpr iterator find(const Key& key) noexcept {
    const auto index = find_index(key);
    if (index.has_value() == false) {
      return {};
    }

    return {data_.begin() + *index, std::ssize(*this)};
  }

  constexpr iterator begin() noexcept {
    return {data_.begin(), std::ssize(*this)};
  }

  constexpr const_iterator begin() const noexcept {
    return {data_.begin(), std::ssize(*this)};
  }

  constexpr auto end() noexcept {
    return std::default_sentinel;
  }

  constexpr auto end() const noexcept {
    return std::default_sentinel;
  }

 private:
  std::array<std::optional<value_type>, MaxSize> data_{};
  std::size_t size_ = 0;

  constexpr std::optional<std::size_t> find_index(
      const Key& key) const noexcept {
    for (std::size_t i = 0; i < size_; ++i) {
      if (data_[i]->first == key) {
        return i;
      }
    }

    return std::nullopt;
  }
};

}  // namespace injectx::stdext
