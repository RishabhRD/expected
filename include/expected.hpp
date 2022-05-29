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
      std::is_nothrow_swappable_v<E>) requires(std::is_swappable_v<E>) {
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

  friend constexpr void swap(unexpected& x, unexpected& y) noexcept(
      noexcept(x.swap(y))) requires(std::is_swappable_v<E>) {
    x.swap(y);
  }

 private : E val;
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

// This function makes sure expected doesn't get into valueless_by_exception
// state due to any exception while assignment
template <class T, class U, class... Args>
constexpr void reinit_expected(T& newval, U& oldval, Args&&... args) {
  if constexpr (std::is_nothrow_constructible_v<T, Args...>) {
    std::destroy_at(std::addressof(oldval));
    std::construct_at(std::addressof(newval), std::forward<Args>(args)...);
  } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
    T tmp(std::forward<Args>(args)...);
    std::destroy_at(std::addressof(oldval));
    std::construct_at(std::addressof(newval), std::move(tmp));
  } else {
    U tmp(std::move(oldval));
    std::destroy_at(std::addressof(oldval));
    try {
      std::construct_at(std::addressof(newval), std::forward<Args>(args)...);
    } catch (...) {
      std::construct_at(std::addressof(oldval), std::move(tmp));
      throw;
    }
  }
}
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
  constexpr auto operator=(expected const& rhs)              // NOLINT
      -> expected& requires std::is_copy_assignable_v<T> &&  //
      std::is_copy_constructible_v<T> &&                     //
      std::is_copy_assignable_v<E> &&                        //
      std::is_copy_constructible_v<E> &&                     //
      (std::is_nothrow_move_constructible_v<E> ||            //
       std::is_nothrow_move_constructible_v<T>)              //
  {
    if (this->has_value() and rhs.has_value()) {
      this->val = *rhs;
    } else if (this->has_value()) {
      detail::reinit_expected(this->unex, this->val, rhs.error());
    } else if (rhs.has_value()) {
      detail::reinit_expected(this->val, this->unex, *rhs);
    } else {
      this->unex = rhs.error();
    }
    return *this;
  }

  constexpr auto operator=(expected&& rhs)  //
      noexcept(std::is_nothrow_move_assignable_v<T>&&
                   std::is_nothrow_move_constructible_v<T>&&
                       std::is_nothrow_move_assignable_v<E>&&
                           std::is_nothrow_move_constructible_v<E>)
          -> expected& requires std::is_move_constructible_v<T> &&  //
      std::is_move_assignable_v<T> &&                               //
      std::is_move_constructible_v<E> &&                            //
      std::is_move_assignable_v<E> &&                               //
      (std::is_nothrow_move_constructible_v<T> ||
       std::is_nothrow_move_constructible_v<E>)  //
  {
    if (this->has_value() and rhs.has_value()) {
      this->val = std::move(*rhs);
    } else if (this->has_value()) {
      detail::reinit_expected(this->unex, this->val, std::move(rhs.error()));
    } else if (rhs.has_value()) {
      detail::reinit_expected(this->val, this->unex, std::move(*rhs));
    } else {
      this->unex = std::move(rhs.error());
    }
    return *this;
  }

  template <class U = T>
      constexpr auto operator=(U&& rhs) -> expected& requires(
          !std::same_as<expected, std::remove_cvref_t<U>>) &&  //
      (!detail::is_unexpected<std::remove_cvref_t<U>>)&&       //
      std::constructible_from<T, U>&&                          //
      std::is_assignable_v<T&, U> &&                           //
      (std::is_nothrow_constructible_v<T, U> ||                //
       std::is_nothrow_move_constructible_v<T> ||              //
       std::is_nothrow_move_constructible_v<E>                 //
      ) {
    if (this->has_value()) {
      this->val = std::forward<U>(rhs);
      return *this;
    }
    detail::reinit_expected(this->val, this->unex, std::forward<U>(rhs));
    has_val = true;
    return *this;
  }

  template <class G>
  requires std::constructible_from<E, G const&> &&      // NOLINT
      std::is_assignable_v<E&, G const&> &&             // NOLINT
      (std::is_nothrow_constructible_v<E, G const&> ||  // NOLINT
       std::is_nothrow_move_constructible_v<T> ||       // NOLINT
       std::is_nothrow_move_constructible_v<E>          // NOLINT
       ) constexpr auto
      operator=(unexpected<G> const& e) -> expected& {
    using GF = G const&;
    if (has_value()) {
      detail::reinit_expected(this->unex, this->val,
                              std::forward<GF>(e.value()));
    } else {
      this->unex = std::forward<GF>(e.value());
    }
    return *this;
  }

  template <class G>
  requires std::constructible_from<E, G> &&        // NOLINT
      std::is_assignable_v<E&, G> &&               // NOLINT
      (std::is_nothrow_constructible_v<E, G> ||    // NOLINT
       std::is_nothrow_move_constructible_v<T> ||  // NOLINT
       std::is_nothrow_move_constructible_v<E>     // NOLINT
       ) constexpr auto
      operator=(unexpected<G>&& e) -> expected& {
    using GF = G;
    if (has_value()) {
      detail::reinit_expected(this->unex, this->val,
                              std::forward<GF>(e.value()));
    } else {
      this->unex = std::forward<GF>(e.value());
    }
    return *this;
  }

  // modifiers
  template <class... Args>
  requires std::is_nothrow_constructible_v<T, Args...>
  constexpr auto emplace(Args&&... args) noexcept -> T& {
    if (has_value()) {
      std::destroy_at(std::addressof(this->val));
    } else {
      std::destroy_at(std::addressof(this->unex));
      has_val = true;
    }
    return *std::construct_at(std::addressof(this->val),
                              std::forward<Args>(args)...);
  }

  template <class U, class... Args>
  requires std::is_nothrow_constructible_v < T, std::initializer_list<U>
  &, Args... > constexpr auto emplace(std::initializer_list<U> il,
                                      Args&&... args) noexcept -> T& {
    if (has_value()) {
      std::destroy_at(std::addressof(this->val));
    } else {
      std::destroy_at(std::addressof(this->unex));
      has_val = true;
    }
    return *std::construct_at(std::addressof(this->val), il,
                              std::forward<Args>(args)...);
  }

  // swap
  // TODO: noexcept clause
  constexpr void swap(expected& rhs) noexcept(
      std::is_nothrow_constructible_v<T>&& std::is_nothrow_swappable_v<T>&&
          std::is_nothrow_move_constructible_v<E>&& std::is_nothrow_swappable_v<
              E>) requires std::is_swappable_v<T> &&  //
      std::is_swappable_v<E> &&                       //
      std::is_move_constructible_v<T> &&              //
      std::is_move_constructible_v<E> &&              //
      (std::is_nothrow_constructible_v<T> ||          //
       std::is_nothrow_constructible_v<E>)            //
  {
    if (rhs.has_value()) {
      if (has_value()) {
        using std::swap;
        swap(this->val, rhs.val);
      } else {
        rhs.swap(*this);
      }
    } else {
      if (has_value()) {
        if constexpr (std::is_nothrow_move_constructible_v<E>) {
          E tmp(std::move(rhs.unex));
          std::destroy_at(std::addressof(rhs.unex));
          try {
            std::construct_at(std::addressof(rhs.val), std::move(this->val));
            std::destroy_at(std::addressof(this->val));
            std::construct_at(std::addressof(this->unex), std::move(tmp));
          } catch (...) {
            std::construct_at(std::addressof(rhs.unex), std::move(tmp));
            throw;
          }
        } else {
          T tmp(std::move(this->val));
          std::destroy_at(std::addressof(this->val));
          try {
            std::construct_at(std::addressof(this->unex), std::move(rhs.unex));
            std::destroy_at(std::addressof(rhs.unex));
            std::construct_at(std::addressof(rhs.val), std::move(tmp));
          } catch (...) {
            std::construct_at(std::addressof(this->val), std::move(tmp));
            throw;
          }
        }
        has_val = false;
        rhs.has_val = true;
      } else {
        using std::swap;
        swap(this->unex, rhs.unex);
      }
    }
  }

  // observers

  // precondition: has_value() = true
  constexpr auto operator->() const noexcept -> T const* {
    return std::addressof(this->val);
  }

  // precondition: has_value() = true
  constexpr auto operator->() noexcept -> T* {
    return std::addressof(this->val);
  }

  // precondition: has_value() = true
  constexpr auto operator*() const& noexcept -> T const& { return this->val; }

  // precondition: has_value() = true
  constexpr auto operator*() & noexcept -> T& { return this->val; }

  // precondition: has_value() = true
  constexpr auto operator*() const&& noexcept -> T const&& {
    return std::move(this->val);
  }

  // precondition: has_value() = true
  constexpr auto operator*() && noexcept -> T&& { return std::move(this->val); }

  constexpr explicit operator bool() const noexcept { return has_val; }

  [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
    return has_val;
  }

  constexpr auto value() const& -> T const& {
    if (has_value()) {
      return this->val;
    }
    throw bad_expected_access(error());
  }

  constexpr auto value() & -> T& {
    if (has_value()) {
      return this->val;
    }
    throw bad_expected_access(error());
  }

  constexpr auto value() const&& -> T const&& {
    if (has_value()) {
      return std::move(this->val);
    }
    throw bad_expected_access(std::move(error()));
  }

  constexpr auto value() && -> T&& {
    if (has_value()) {
      return std::move(this->val);
    }
    throw bad_expected_access(std::move(error()));
  }

  // precondition: has_value() = false
  constexpr auto error() const& -> E const& { return this->unex; }

  // precondition: has_value() = false
  constexpr auto error() & -> E& { return this->unex; }

  // precondition: has_value() = false
  constexpr auto error() const&& -> E const&& { return std::move(this->unex); }

  // precondition: has_value() = false
  constexpr auto error() && -> E&& { return std::move(this->unex); }

  template <class U>
  requires std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>
  constexpr auto value_or(U&& v) const& -> T {
    return has_value() ? **this : static_cast<T>(std::forward<U>(v));
  }

  template <class U>
  requires std::is_move_constructible_v<T> && std::is_convertible_v<U, T>
  constexpr auto value_or(U&& v) && -> T {
    return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(v));
  }

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
  friend constexpr void swap(expected& x,
                             expected& y) noexcept(noexcept(x.swap(y))) {
    x.swap(y);
  }

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
