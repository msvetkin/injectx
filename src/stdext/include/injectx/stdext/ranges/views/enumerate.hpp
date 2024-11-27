// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace injectx::stdext::ranges::views {

namespace details::_enumerate {

template<typename Iterator>
class [[nodiscard]] iterator {
 public:
  using value_type =
      std::pair<std::size_t, decltype(*std::declval<Iterator>())>;
  using difference_type = std::ptrdiff_t;
  using iterator_category =
      typename std::iterator_traits<Iterator>::iterator_category;

  constexpr explicit iterator(Iterator it, std::size_t idx = 0)
      : current_(it),
        index_(idx) {
  }

  constexpr value_type operator*() const {
    return {index_, *current_};
  }

  constexpr iterator &operator++() {
    ++current_;
    ++index_;
    return *this;
  }

  constexpr iterator operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  constexpr bool operator==(const iterator &other) const {
    return current_ == other.current_;
  }

  constexpr bool operator!=(const iterator &other) const {
    return !(*this == other);
  }

 private:
  Iterator current_;
  std::size_t index_;
};

// Helper function to create an iterator.
template<typename Container>
class [[nodiscard]] wrapper {
 public:
  constexpr explicit wrapper(const Container &container)
      : container_(container) {
  }

  wrapper(const wrapper &other) noexcept
      : container_(other.container_) {
  }

  wrapper &operator=(const wrapper &other) = delete;

  wrapper(wrapper &&other) noexcept
      : container_(other.container_) {
  }

  wrapper &operator=(Container &&other) = delete;

  constexpr auto begin() const {
    if (container_.empty()) {
      return end();
    }

    return iterator{container_.begin()};
  }

  constexpr auto end() const {
    return iterator{std::end(container_), std::size(container_)};
  }

 private:
  const Container &container_;
};

}  // namespace details::_enumerate

struct enumerate_fn {
  template<typename Container>
  constexpr auto operator()(const Container &c) const noexcept {
    return details::_enumerate::wrapper<Container>(c);
  }
};

inline constexpr enumerate_fn enumerate{};

}  // namespace injectx::stdext::ranges::views

template<typename Container>
[[nodiscard]] constexpr auto operator|(
    const Container &c,
    const injectx::stdext::ranges::views::enumerate_fn &f) noexcept {
  return f(c);
}
