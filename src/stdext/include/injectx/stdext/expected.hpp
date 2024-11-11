// SPDX-FileCopyrightText: Copyright 2024 Mikhail Svetkin
// SPDX-License-Identifier: MIT

#pragma once

#include "injectx/stdext/expects.hpp"

#include <concepts>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

namespace injectx::stdext {

namespace details::_expected {

template<typename Error>
concept valid_error_type = !std::is_void_v<Error>;

template<typename T, typename Error>
concept valid_value_and_error_types = requires {
  requires valid_error_type<Error>;
  requires !std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<Error>>;
};

}  // namespace details::_expected

template<details::_expected::valid_error_type Error>
class [[nodiscard]] unexpected {
 public:
  template<typename E = Error>
    requires std::constructible_from<Error, E>
  constexpr explicit unexpected(E&& error)
      : error_(std::forward<E>(error)) {
  }

  [[nodiscard]] constexpr Error& error() & noexcept {
    return error_;
  }

  [[nodiscard]] constexpr const Error& error() const& noexcept {
    return error_;
  }

  [[nodiscard]] constexpr Error&& error() && noexcept {
    return std::move(error_);
  }

  [[nodiscard]] constexpr const Error&& error() const&& noexcept {
    return std::move(error_);
  }

 private:
  Error error_;
};

template<typename Error>
unexpected(Error) -> unexpected<Error>;

template<typename T, typename Error>
  requires details::_expected::valid_value_and_error_types<T, Error>
class [[nodiscard]] expected {
 public:
  using value_type = T;
  using error_type = Error;
  using unexpected_type = unexpected<error_type>;

  template<typename V>
  using rebind = expected<V, error_type>;

  template<typename E>
  using rebind_error = expected<T, E>;

  constexpr expected(const expected& other)
      : value_(other.value_) {
  }

  constexpr expected(expected&& other) noexcept
      : value_(std::move(other.value_)) {
  }

  template<typename V = T>
  constexpr explicit(!std::is_convertible_v<V, T>) expected(V&& v)
      : value_(std::forward<V>(v)) {
  }

  template<typename E = error_type>
  constexpr explicit(!std::convertible_to<E, error_type>)
      expected(unexpected<E>&& unexp)
      : value_(unexp.error()) {
  }

  [[nodiscard]] constexpr value_type& value() & noexcept {
    expects(has_value());
    return std::get<value_type>(value_);
  }

  [[nodiscard]] constexpr const value_type& value() const& noexcept {
    expects(has_value());
    return std::get<value_type>(value_);
  }

  [[nodiscard]] constexpr value_type&& value() && noexcept {
    expects(has_value());
    return std::get<value_type>(std::move(value_));
  }

  [[nodiscard]] constexpr const value_type&& value() const&& noexcept {
    expects(has_value());
    return std::get<value_type>(std::move(value_));
  }

  [[nodiscard]] constexpr error_type& error() & noexcept {
    expects(has_value() == false);
    return std::get<error_type>(value_);
  }

  [[nodiscard]] constexpr const error_type& error() const& noexcept {
    expects(has_value() == false);
    return std::get<error_type>(value_);
  }

  [[nodiscard]] constexpr error_type&& error() && noexcept {
    expects(has_value() == false);
    return std::get<error_type>(std::move(value_));
  }

  [[nodiscard]] constexpr const error_type&& error() const&& noexcept {
    expects(has_value() == false);
    return std::get<error_type>(std::move(value_));
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return std::holds_alternative<value_type>(value_);
  }

  [[nodiscard]] constexpr value_type& operator*() & noexcept {
    expects(has_value());
    return value();
  }

  [[nodiscard]] constexpr const value_type& operator*() const& noexcept {
    expects(has_value());
    return value();
  }

  [[nodiscard]] constexpr const value_type&& operator*() const&& noexcept {
    expects(has_value());
    return value();
  }

  [[nodiscard]] constexpr value_type&& operator*() && noexcept {
    expects(has_value());
    return value();
  }

  [[nodiscard]] constexpr auto operator->() noexcept {
    expects(has_value());
    return &value();
  }

  [[nodiscard]] constexpr auto operator->() const noexcept {
    expects(has_value());
    return &value();
  }

 private:
  std::variant<value_type, error_type> value_;
};

template<details::_expected::valid_error_type Error>
class [[nodiscard]] expected<void, Error> {
 public:
  using value_type = void;
  using error_type = Error;
  using unexpected_type = unexpected<error_type>;

  template<typename V>
  using rebind = expected<V, error_type>;

  template<typename E>
  using rebind_error = expected<value_type, E>;

  constexpr expected() = default;

  constexpr expected(std::in_place_t) noexcept {
  }

  template<typename E = Error>
  constexpr explicit(!std::convertible_to<E, Error>)
      expected(unexpected<E>&& unexp)
      : error_(std::forward<unexpected<E>>(unexp).error()) {
  }

  constexpr void value() const noexcept {
    expects(has_value());
    return;
  }

  [[nodiscard]] constexpr error_type& error() & noexcept {
    expects(has_value() == false);
    return error_.value();
  }

  [[nodiscard]] constexpr const error_type& error() const& noexcept {
    expects(has_value() == false);
    return error_.value();
  }

  [[nodiscard]] constexpr error_type&& error() && noexcept {
    expects(has_value() == false);
    return std::move(error_).value();
  }

  [[nodiscard]] constexpr const error_type&& error() const&& noexcept {
    expects(!has_value());
    return std::move(error_).value();
  }

  [[nodiscard]] constexpr bool has_value() const noexcept {
    return error_.has_value() == false;
  }

  constexpr void operator*() const noexcept {
    expects(has_value());
    return;
  }

 private:
  std::optional<Error> error_;
};

namespace details::_expected {

template<typename T>
inline constexpr bool is_expected = false;

template<typename T, typename E>
inline constexpr bool is_expected<expected<T, E>> = true;

}  // namespace details::_expected

template<typename T>
concept is_expected = details::_expected::is_expected<std::remove_cvref_t<T>>;

}  // namespace injectx::stdext
