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

TEST_CASE("operator-> non-const") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(ex->front() == 'h');
}

TEST_CASE("operator-> const") {
  rd::expected<std::string, std::string> const ex{"hello"};
  REQUIRE(ex->front() == 'h');
}

TEST_CASE("operator* non-const lvalue") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(*ex == "hello");
}

TEST_CASE("operator* const lvalue") {
  rd::expected<std::string, std::string> const ex{"hello"};
  REQUIRE(*ex == "hello");
}

TEST_CASE("operator* rvalue") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(*(rd::expected<std::string, std::string>{"hello"}) == "hello");
}

TEST_CASE("operator bool with value") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(ex);
}

TEST_CASE("operator bool with error") {
  rd::expected<std::string, std::string> ex{rd::unexpect, "hello"};
  REQUIRE(!ex);
}

TEST_CASE("has_value with value") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(ex.has_value());
}

TEST_CASE("has_value with error") {
  rd::expected<std::string, std::string> ex{rd::unexpect, "hello"};
  REQUIRE(!ex.has_value());
}

TEST_CASE(".value") {
  rd::expected<std::string, std::string> ex{"hello"};
  REQUIRE(ex.value() == "hello");
}

TEST_CASE(".error") {
  rd::expected<std::string, std::string> ex{rd::unexpect, "hello"};
  REQUIRE(ex.error() == "hello");
}

TEST_CASE(".value_or with value") {
  rd::expected<std::string, std::string> ex{"value"};
  REQUIRE(ex.value_or("new value") == "value");
}

TEST_CASE(".value_or with value") {
  rd::expected<std::string, std::string> ex{rd::unexpect, "value"};
  REQUIRE(ex.value_or("new value") == "new value");
}
