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
#include <type_traits>
#include <utility>
#include <variant>

namespace rd {

template <class E>
class unexpected {
  // TODO: figure out if move constructor and move assignment needs to be
  // noexcept
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
class bad_expected_access<void>;

struct unexpect_t {
  explicit unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

template <class T, class E>
class expected;

template <class E>
class expected<void, E>;

}  // namespace rd
