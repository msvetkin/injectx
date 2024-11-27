// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expected.hpp"

#include <functional>
#include <string_view>

namespace injectx::stdext {

template<typename T, std::size_t MaxSize>
class static_queue {
  using FrontExpected =
      expected<std::reference_wrapper<const T>, std::string_view>;

 public:
  using container_type = std::array<T, MaxSize>;
  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;

  constexpr expected<void, std::string_view> push(const T& value) noexcept {
    if (size_ >= MaxSize) {
      return unexpected{"queue is full"};
    }

    data_[tail_] = value;
    tail_ = (tail_ + 1) % MaxSize;
    ++size_;

    return {};
  }

  constexpr expected<T, std::string_view> pop() noexcept {
    if (size_ == 0) {
      return unexpected{std::string_view{"queue is empty"}};
    }

    T value = data_[head_];
    head_ = (head_ + 1) % MaxSize;
    --size_;
    return value;
  }

  constexpr FrontExpected front() const noexcept {
    if (size_ == 0) {
      return unexpected{"queue is empty"};
    }

    return data_[head_];
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return size_ == 0;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return size_;
  }

  [[nodiscard]] constexpr std::size_t max_size() const noexcept {
    return MaxSize;
  }

 private:
  container_type data_{};
  std::size_t head_{0};
  std::size_t tail_{0};
  std::size_t size_{0};
};

}  // namespace injectx::stdext
