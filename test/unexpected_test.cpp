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

#include <utility>

#include "test_include.hpp"

TEST_CASE("error constructor test") {
  rd::unexpected<int> un(2);
  REQUIRE(un.value() == 2);
}

TEST_CASE("inplace constructor test") {
  rd::unexpected<int> un(std::in_place, 2);
  REQUIRE(un.value() == 2);
}

TEST_CASE("copy constructor test") {
  rd::unexpected<int> u_orig{2};
  rd::unexpected<int> un(u_orig);
  REQUIRE(un.value() == 2);
}

TEST_CASE("move constructor test") {
  rd::unexpected<int> un(rd::unexpected<int>{2});
  REQUIRE(un.value() == 2);
}

TEST_CASE("swap test") {
  rd::unexpected<int> lhs(2);
  rd::unexpected<int> rhs(3);
  using std::swap;
  swap(lhs, rhs);
  REQUIRE(lhs.value() == 3);
  REQUIRE(rhs.value() == 2);
}

TEST_CASE("equality operator test") {
  rd::unexpected<int> lhs(2);
  rd::unexpected<int> rhs(2);
  REQUIRE(lhs == rhs);
}
