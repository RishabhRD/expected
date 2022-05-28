/*
 * MIT License
 *
 * Copyright (c) 2022 Rishabh Dwivedi<rishabhdwivedi17@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// TODO: manual formatting of file or improve .clang-format

#pragma once

#include <concepts>
#include <deque>
#include <exception>
#include <memory>
#include <type_traits>
#include <utility>
#include <variant>

namespace rd {

template <class E>
class unexpected {
  // TODO: figure out if move constructor and move assignment needs to be
  // noexcept
  using error_type = E;
  constexpr unexpected(unexpected const&) = default;
  constexpr unexpected(unexpected&&) = default;  // NOLINT
  constexpr auto operator=(unexpected const&) -> unexpected& = default;
  constexpr auto operator=(unexpected&&) -> unexpected& = default;  // NOLINT
  ~unexpected() = default;

  template <class... Args>
  requires std::constructible_from<E, Args...>
  constexpr explicit unexpected(std::in_place_t /*unused*/, Args&&... args)
      : val(std::forward<Args>(args)...) {}

  template <class U, class... Args>
  requires std::constructible_from < E, std::initializer_list<U>
  &, Args... > constexpr explicit unexpected(std::in_place_t /*unused*/,
                                             std::initializer_list<U> i_list,
                                             Args&&... args)
      : val(i_list, std::forward<Args>(args)...) {}

  template <class Err = E>
  requires                                                          //
      (!std::same_as<std::remove_cvref_t<Err>, unexpected>) &&      //
      (!std::same_as<std::remove_cvref_t<Err>, std::in_place_t>)&&  //
      std::constructible_from<E, Err>                               //
      constexpr explicit unexpected(Err&& err) : val(std::forward<Err>(err)) {}

  constexpr auto value() const& noexcept -> E const& { return val; }
  constexpr auto value() & noexcept -> E& { return val; }
  constexpr auto value() const&& noexcept -> E const&& {
    return std::move(val);
  }
  constexpr auto value() && noexcept -> E&& { return std::move(val); }

  constexpr void swap(unexpected& other) noexcept(
      std::is_nothrow_swappable_v<E>) {
    static_assert(std::is_swappable_v<E>);
    using std::swap;
    swap(val, other.val);
  }

  template <class E2>
  requires(requires(unexpected const& x, unexpected<E2> const& y) {
    { x.value() == y.value() } -> std::convertible_to<bool>;
  })  //
      friend constexpr auto
      operator==(unexpected const& x, unexpected<E2> const& y) -> bool {
    return x.value() == y.value();
  }

  friend constexpr void swap(unexpected& x,
                             unexpected& y) noexcept(noexcept(x.swap(y))) {
    static_assert(std::is_swappable_v<E>);
    x.swap(y);
  }

 private:
  E val;
};

template <class E>
unexpected(E) -> unexpected<E>;

template <class E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
 protected:
  bad_expected_access() noexcept = default;
  bad_expected_access(bad_expected_access const&) = default;
  bad_expected_access(bad_expected_access&&) = default;
  auto operator=(bad_expected_access const&) -> bad_expected_access& = default;
  auto operator=(bad_expected_access&&) -> bad_expected_access& = default;
  ~bad_expected_access() override = default;

 public:
  auto what() const noexcept -> char const* override {  // NOLINT
    return "bad expected access";
  }
};

template <class E>
class bad_expected_access : public bad_expected_access<void> {
 public:
  explicit bad_expected_access(E e) : val(std::move(e)) {}
  auto what() const noexcept -> char const* override {  // NOLINT
    return "bad expected access";
  }

  auto error() & noexcept -> E& { return val; }
  auto error() const& noexcept -> E const& { return val; }
  auto error() && noexcept -> E&& { return std::move(val); }
  auto error() const&& noexcept -> const E&& { return std::move(val); }

 private:
  E val;
};

struct unexpect_t {};
inline constexpr unexpect_t unexpect{};

template <class T, class E>
class expected;

namespace detail {
template <typename T, typename E, typename U, typename G, typename UF,
          typename GF>
concept expected_constructible_from_other =
    std::constructible_from<T, UF> &&                                   //
    std::constructible_from<E, GF> &&                                   //
    (!std::constructible_from<T, expected<U, G>&>)&&                    //
    (!std::constructible_from<T, expected<U, G>>)&&                     //
    (!std::constructible_from<T, expected<U, G> const&>)&&              //
    (!std::constructible_from<T, expected<U, G> const>)&&               //
    (!std::convertible_to<expected<U, G>&, T>)&&                        //
    (!std::convertible_to<expected<U, G>&&, T>)&&                       //
    (!std::convertible_to<expected<U, G> const&, T>)&&                  //
    (!std::convertible_to<expected<U, G> const&&, T>)&&                 //
    (!std::constructible_from<unexpected<E>, expected<U, G>&>)&&        //
    (!std::constructible_from<unexpected<E>, expected<U, G>>)&&         //
    (!std::constructible_from<unexpected<E>, expected<U, G> const&>)&&  //
    (!std::constructible_from<unexpected<E>, expected<U, G> const>);

template <typename T>
concept is_unexpected_non_void = requires(T) {
  typename T::error_type;
  std::same_as<typename T::error_type, unexpected<typename T::error_type>>;
};

template <typename T>
concept is_unexpected =
    std::same_as<T, unexpected<void>> || is_unexpected_non_void<T>;
}  // namespace detail

template <class T, class E>
class expected {
  // TODO: T should follow destructible
 public:
  using value_type = T;
  using error_type = E;
  using unexpected_type = unexpected<E>;

  template <class U>
  using rebind = expected<U, error_type>;

  // constructors
  constexpr expected() requires std::default_initializable<T>
      : has_val{true}, val{} {};

  constexpr expected(expected const& rhs)  //
      requires std::copy_constructible<T> && std::copy_constructible<E>
  = default;

  constexpr expected(expected&&) noexcept(
      std::is_nothrow_move_constructible_v<T>&&
          std::is_nothrow_move_constructible_v<T>) requires
      std::move_constructible<T> && std::move_constructible<E>
  = default;

  template <class U, class G>
  requires detail::expected_constructible_from_other<T, E, U, G, U const&,
                                                     G const&>
  constexpr explicit(!std::convertible_to<U const&, T> ||
                     !std::convertible_to<G const&, E>)
      expected(expected<U, G> const& rhs) {  // NOLINT
    using UF = U const&;
    using GF = G const&;
    if (rhs.has_value()) {
      construct(std::forward<UF>(*rhs));
    } else {
      construct_error(std::forward<GF>(rhs.error()));
    }
  }  // NOLINT

  template <class U, class G>
  requires detail::expected_constructible_from_other<T, E, U, G, U, G>
  constexpr explicit(!std::convertible_to<U const&, T> ||
                     !std::convertible_to<G const&, E>)
      expected(expected<U, G>&& rhs) {  // NOLINT
    using UF = U;
    using GF = G;
    if (rhs.has_value()) {
      construct(std::forward<UF>(*rhs));
    } else {
      construct_error(std::forward<GF>(rhs.error()));
    }
  }

  template <class U = T>
  requires(!std::same_as<std::remove_cvref_t<U>, std::in_place_t>) &&  //
      (!std::same_as<expected<T, E>, std::remove_cvref_t<U>>)&&        //
      (!detail::is_unexpected<U>)&&                                    //
      std::constructible_from<T, U>                                    //
      constexpr explicit(!std::convertible_to<U, T>) expected(U&& v)   // NOLINT
      : expected(std::in_place, std::forward<U>(v)) {}

  template <class G>
  requires std::constructible_from<E, G const&>
  constexpr explicit(!std::convertible_to<G const&, E>)
      expected(unexpected<G>& e)  // NOLINT
      : expected(unexpect, e.error()) {}

  template <class G>
  requires std::constructible_from<E, G>
  constexpr explicit(!std::convertible_to<G, E>)
      expected(unexpected<G>&& e)  // NOLINT
      : expected(unexpect, std::forward<G>(e.error())) {}

  template <class... Args>
  requires std::constructible_from<T, Args...>
  constexpr explicit expected(std::in_place_t /*unused*/, Args&&... args) {
    construct(std::forward<Args>(args)...);
  }

  template <class U, class... Args>
  requires std::constructible_from < T, std::initializer_list<U>
  &, Args... > constexpr explicit expected(std::in_place_t /*unused*/,
                                           std::initializer_list<U> il,
                                           Args&&... args) {
    construct(il, std::forward<Args>(args)...);
  }

  template <class... Args>
  requires std::constructible_from<E, Args...>
  constexpr explicit expected(unexpect_t /*unused*/, Args&&... args) {
    construct_error(std::forward<Args>(args)...);
  }

  template <class U, class... Args>
  requires std::constructible_from < E, std::initializer_list<U>
  &, Args... > constexpr explicit expected(unexpect_t /*unused*/,
                                           std::initializer_list<U> il,
                                           Args&&... args) {
    construct_error(il, std::forward<Args>(args)...);
  }

  // destructor
  constexpr ~expected() {
    if constexpr (std::is_trivially_destructible_v<T> and
                  std::is_trivially_destructible_v<E>) {
    } else if constexpr (std::is_trivially_destructible_v<T>) {
      if (!has_val) {
        std::destroy_at(std::addressof(this->unex));
      }
    } else if constexpr (std::is_trivially_destructible_v<E>) {
      if (has_val) {
        std::destroy_at(std::addressof(this->val));
      }
    } else {
      if (has_val) {
        std::destroy_at(std::addressof(this->val));
      } else {
        std::destroy_at(std::addressof(this->unex));
      }
    }
  }

  // assignment
  constexpr auto operator=(expected const&) -> expected&;
  // TODO: noexcept clause
  constexpr auto operator=(expected&&) noexcept -> expected&;
  template <class U = T>
  constexpr auto operator=(U&&) -> expected&;
  template <class G>
  constexpr auto operator=(unexpected<G> const&) -> expected&;
  template <class G>
  constexpr auto operator=(unexpected<G>&&) -> expected&;

  // modifiers
  template <class... Args>
  constexpr auto emplace(Args&&...) noexcept -> T&;
  template <class U, class Args>
  constexpr auto emplace(std::initializer_list<U>, Args&&...) noexcept -> T&;

  // swap
  // TODO: noexcept clause
  constexpr void swap(expected&) noexcept;

  // observers
  constexpr auto operator->() const noexcept -> T const*;
  constexpr auto operator->() noexcept -> T*;
  constexpr auto operator*() const& noexcept -> T const&;
  constexpr auto operator*() & noexcept -> T&;
  constexpr auto operator*() const&& noexcept -> T const&&;
  constexpr auto operator*() && noexcept -> T&&;
  constexpr explicit operator bool() const noexcept;
  constexpr auto has_value() const noexcept -> bool;  // NOLINT
  constexpr auto value() const& -> T const&;
  constexpr auto value() & -> T&;
  constexpr auto value() const&& -> T const&&;
  constexpr auto value() && -> T&&;
  constexpr auto error() const& -> E const&;
  constexpr auto error() & -> E&;
  constexpr auto error() const&& -> E const&&;
  constexpr auto error() && -> E&&;
  template <class U>
  constexpr auto value_or(U&&) const& -> T;
  template <class U>
  constexpr auto value_or(U&&) && -> T;

  // equality operators
  template <class T2, class E2>
  friend constexpr auto operator==(expected const&, expected<T2, E2> const&)
      -> bool;
  template <class T2>
  friend constexpr auto operator==(expected const&, T2 const&) -> bool;
  template <class E2>
  friend constexpr auto operator==(expected const&, unexpected<E2> const&)
      -> bool;

  // specialized algorithms
  // TODO: noexcept clause
  friend constexpr void swap(expected&, expected&) noexcept;

 private:
  template <class... Args>
  void construct(Args&&... args) noexcept {
    std::construct_at(std::addressof(this->val), std::forward<Args>(args)...);
    this->has_val = true;
  }

  template <class... Args>
  void construct_error(Args&&... args) noexcept {
    std::construct_at(std::addressof(this->unex), std::forward<Args>(args)...);
    this->has_val = false;
  }

  bool has_val{};
  union {
    T val;
    E unex;
  };
};

template <class E>
class expected<void, E>;

}  // namespace rd
