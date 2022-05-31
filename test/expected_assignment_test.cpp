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

TEST_CASE("copy: lhs has value, rhs has error") {
  rd::expected<std::string, std::string> const rhs{rd::unexpect, "error"};
  rd::expected<std::string, std::string> lhs{"value"};
  lhs = rhs;
  REQUIRE(lhs.has_value() == rhs.has_value());
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "error");
}

TEST_CASE("copy: lhs has error, rhs has value") {
  rd::expected<std::string, std::string> const rhs{"value"};
  rd::expected<std::string, std::string> lhs{rd::unexpect, "error"};
  lhs = rhs;
  REQUIRE(lhs.has_value() == rhs.has_value());
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("copy: lhs has value, rhs has value") {
  rd::expected<std::string, std::string> const rhs{"value"};
  rd::expected<std::string, std::string> lhs{"value1"};
  lhs = rhs;
  REQUIRE(lhs.has_value() == rhs.has_value());
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("copy: lhs has error, rhs has error") {
  rd::expected<std::string, std::string> const rhs{rd::unexpect, "value"};
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  lhs = rhs;
  REQUIRE(lhs.has_value() == rhs.has_value());
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("move: lhs has value, rhs has error") {
  rd::expected<std::string, std::string> rhs{rd::unexpect, "error"};
  auto const rhs_has_value = rhs.has_value();
  rd::expected<std::string, std::string> lhs{"value"};
  lhs = std::move(rhs);
  REQUIRE(lhs.has_value() == rhs_has_value);
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "error");
}

TEST_CASE("move: lhs has error, rhs has value") {
  rd::expected<std::string, std::string> rhs{"value"};
  auto const rhs_has_value = rhs.has_value();
  rd::expected<std::string, std::string> lhs{rd::unexpect, "error"};
  lhs = std::move(rhs);
  REQUIRE(lhs.has_value() == rhs_has_value);
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("move: lhs has value, rhs has value") {
  rd::expected<std::string, std::string> rhs{"value"};
  auto const rhs_has_value = rhs.has_value();
  rd::expected<std::string, std::string> lhs{"value1"};
  lhs = std::move(rhs);
  REQUIRE(lhs.has_value() == rhs_has_value);
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("move: lhs has error, rhs has error") {
  rd::expected<std::string, std::string> rhs{rd::unexpect, "value"};
  auto const rhs_has_value = rhs.has_value();
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  lhs = std::move(rhs);
  REQUIRE(lhs.has_value() == rhs_has_value);
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("assigning values : lhs has value with no conversion") {
  rd::expected<std::string, std::string> lhs{"value1"};
  lhs = std::string{"value"};
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("assigning values : lhs has value conversion") {
  rd::expected<std::string, std::string> lhs{"value1"};
  // TODO: confirm if explicit convertable operation should work
  lhs = int_to_str_implicit(2);
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "2");
}

TEST_CASE("assigning values : lhs has error with no conversion") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  lhs = std::string{"value"};
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "value");
}

TEST_CASE("assigning values : lhs has value conversion") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  // TODO: confirm if explicit convertable operation should work
  lhs = int_to_str_implicit(2);
  REQUIRE(lhs.has_value());
  REQUIRE(*lhs == "2");
}

TEST_CASE("assigning unexpected : lhs has value without conversion") {
  rd::expected<std::string, std::string> lhs{"value1"};
  lhs = rd::unexpected{std::string{"value"}};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("assigning unexpected : lhs has error without conversion") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  lhs = rd::unexpected{std::string{"value"}};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("assigning unexpected : lhs has value with conversion") {
  rd::expected<std::string, std::string> lhs{"value1"};
  lhs = rd::unexpected{int_to_str_implicit{2}};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "2");
}

TEST_CASE("assigning unexpected : lhs has error with conversion") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  lhs = rd::unexpected{int_to_str_implicit{2}};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "2");
}

TEST_CASE("assigning unexpected : lhs has value without conversion COPY") {
  rd::expected<std::string, std::string> lhs{"value1"};
  rd::unexpected rhs{std::string{"value"}};
  lhs = rd::unexpected{rhs};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("assigning unexpected : lhs has error without conversion COPY") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  rd::unexpected rhs{std::string{"value"}};
  lhs = rd::unexpected{rhs};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "value");
}

TEST_CASE("assigning unexpected : lhs has value with conversion COPY") {
  rd::expected<std::string, std::string> lhs{"value1"};
  rd::unexpected rhs{int_to_str_implicit{2}};
  lhs = rd::unexpected{rhs};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "2");
}

TEST_CASE("assigning unexpected : lhs has error with conversion COPY") {
  rd::expected<std::string, std::string> lhs{rd::unexpect, "value1"};
  rd::unexpected rhs{int_to_str_implicit{2}};
  lhs = rd::unexpected{rhs};
  REQUIRE(!lhs.has_value());
  REQUIRE(lhs.error() == "2");
}
