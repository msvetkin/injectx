// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expected.hpp"

#include <array>
#include <optional>
#include <string_view>
#include <utility>

namespace injectx::stdext {

namespace details::_static_map {

template<typename Optional>
class [[nodiscard]] iterator {
  using Value = typename Optional::value_type;

 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::conditional_t<
      std::is_const_v<Optional>,
      std::add_const_t<typename Optional::value_type>,
      typename Optional::value_type>;
  using pointer = value_type*;
  using reference = value_type&;
  using optional_ptr = Optional*;

  constexpr explicit iterator(optional_ptr ptr = nullptr) noexcept
      : ptr_(ptr) {
  }

  constexpr reference operator*() const noexcept {
    return ptr_->value();
  }

  constexpr pointer operator->() const noexcept {
    return &ptr_->value();
  }

  constexpr iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  friend constexpr std::strong_ordering operator<=>(
      const iterator&, const iterator&) = default;

 private:
  optional_ptr ptr_;
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
  using iterator = details::_static_map::iterator<Optional>;
  using const_iterator = details::_static_map::iterator<const Optional>;

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

  [[nodiscard]] constexpr std::size_t index() const noexcept {
    return const_iterator(&data_[size_]);
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
    return iterator(&data_[size_ - 1]);
  }

  [[nodiscard]] constexpr iterator find(const Key& key) noexcept {
    const auto index = find_index(key);
    if (index.has_value() == false) {
      return end();
    }

    return iterator(&data_[*index]);
  }

  constexpr const_iterator find(const Key& key) const noexcept {
    const auto index = find_index(key);
    if (index.has_value() == false) {
      return end();
    }

    return const_iterator(&data_[*index]);
  }

  constexpr iterator begin() noexcept {
    return iterator(&data_[0]);
  }

  constexpr const_iterator begin() const noexcept {
    return const_iterator(&data_[0]);
  }

  constexpr iterator end() noexcept {
    return iterator(&data_[size_]);
  }

  constexpr const_iterator end() const noexcept {
    return const_iterator(&data_[size_]);
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
