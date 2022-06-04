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

#include "test_include.hpp"

auto consume(int /*unused*/) -> void {}

TEST_CASE("transform & returns void") {
  rd::expected<int, int> ex{2};
  auto e = ex.transform(consume);
  REQUIRE(e.has_value());
}

TEST_CASE("transform const & returns void") {
  rd::expected<int, int> const ex{2};
  auto e = ex.transform(consume);
  REQUIRE(e.has_value());
}

TEST_CASE("transform && returns void") {
  rd::expected<int, int> ex{2};
  auto e = std::move(ex).transform(consume);
  REQUIRE(e.has_value());
}

TEST_CASE("transform & returns void with error") {
  rd::expected<int, int> ex{rd::unexpect, 2};
  auto e = ex.transform(consume);
  REQUIRE(!e.has_value());
  REQUIRE(e.error() == 2);
}

TEST_CASE("transform const & returns void with error") {
  rd::expected<int, int> const ex{rd::unexpect, 2};
  auto e = ex.transform(consume);
  REQUIRE(!e.has_value());
  REQUIRE(e.error() == 2);
}

TEST_CASE("transform && returns void with error") {
  rd::expected<int, int> ex{rd::unexpect, 2};
  auto e = std::move(ex).transform(consume);
  REQUIRE(!e.has_value());
  REQUIRE(e.error() == 2);
}
