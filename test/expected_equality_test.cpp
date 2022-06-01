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

TEST_CASE("expected on lhs and rhs, lhs has value, rhs has value") {
  auto const cond = rd::expected<std::string, std::string>("value") ==
                    rd::expected<std::string, std::string>("value");
  REQUIRE(cond);
  auto const cond1 = rd::expected<std::string, std::string>("value") ==
                     rd::expected<std::string, std::string>("value1");
  REQUIRE_FALSE(cond1);
}

TEST_CASE("expected on lhs and rhs, lhs has value, rhs has error") {
  auto const cond =
      rd::expected<std::string, std::string>("value") ==
      rd::expected<std::string, std::string>(rd::unexpect, "value");
  REQUIRE_FALSE(cond);
}

TEST_CASE("expected on lhs and rhs, lhs has error, rhs has value") {
  auto const cond =
      rd::expected<std::string, std::string>(rd::unexpect, "value") ==
      rd::expected<std::string, std::string>("value");
  REQUIRE_FALSE(cond);
}

TEST_CASE("expected on lhs and rhs, lhs has error, rhs has error") {
  auto const cond =
      rd::expected<std::string, std::string>(rd::unexpect, "value") ==
      rd::expected<std::string, std::string>(rd::unexpect, "value");
  REQUIRE(cond);
  auto const cond1 =
      rd::expected<std::string, std::string>(rd::unexpect, "value") ==
      rd::expected<std::string, std::string>(rd::unexpect, "value1");
  REQUIRE_FALSE(cond1);
}

TEST_CASE("expected on lhs and rhs has value, lhs has value") {
  auto const cond = rd::expected<std::string, std::string>("value") == "value";
  REQUIRE(cond);
  auto const cond1 =
      rd::expected<std::string, std::string>("value") == "value1";
  REQUIRE_FALSE(cond1);
}

TEST_CASE("expected on lhs and rhs has value, lhs has error") {
  auto const cond =
      rd::expected<std::string, std::string>(rd::unexpect, "value") == "value";
  REQUIRE_FALSE(cond);
}

TEST_CASE("expected on lhs and rhs has unexpected, lhs has value") {
  auto const cond = rd::expected<std::string, std::string>("value") ==
                    rd::unexpected{"value"};
  REQUIRE_FALSE(cond);
}

TEST_CASE("expected on lhs and rhs has unexpected, lhs has error") {
  auto const cond = rd::expected<std::string, std::string>(
                        rd::unexpect, "value") == rd::unexpected{"value"};
  REQUIRE(cond);
  auto const cond1 = rd::expected<std::string, std::string>(
                         rd::unexpect, "value") == rd::unexpected{"value1"};
  REQUIRE_FALSE(cond1);
}
