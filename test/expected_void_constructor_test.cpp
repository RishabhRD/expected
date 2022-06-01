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

#include <iostream>

#include "test_include.hpp"

TEST_CASE("default constructor") {
  rd::expected<void, std::string> ex;
  REQUIRE(ex.has_value());
}

TEST_CASE("copy constructor, rhs has value") {
  rd::expected<void, std::string> rhs;
  rd::expected<void, std::string> lhs(rhs);
  REQUIRE(lhs.has_value());
}

TEST_CASE("copy constructor, rhs has error") {
  rd::expected<void, std::string> rhs{rd::unexpect, "error"};
  rd::expected<void, std::string> lhs(rhs);
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "error");
}

TEST_CASE("move constructor, rhs has value") {
  rd::expected<void, std::string> rhs;
  rd::expected<void, std::string> lhs(std::move(rhs));
  REQUIRE(lhs.has_value());
}

TEST_CASE("move constructor, rhs has error") {
  rd::expected<void, std::string> rhs{rd::unexpect, "error"};
  rd::expected<void, std::string> lhs(std::move(rhs));
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "error");
}