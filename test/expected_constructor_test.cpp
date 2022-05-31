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

struct int_to_str {
 private:
  int mem;

 public:
  explicit int_to_str(int m) : mem(m) {}

  operator std::string() const { return std::to_string(mem); }  // NOLINT
};

TEST_CASE("default constructor") {
  rd::expected<std::string, int> ex;
  REQUIRE(ex.has_value());
  REQUIRE(*ex == "");
}

TEST_CASE("Value constructor") {
  rd::expected<std::string, int> const ex{"test value"};
  REQUIRE(ex.has_value());
  REQUIRE(*ex == "test value");
}

TEST_CASE("trivial copy constructor with value") {
  rd::expected<int, int> const orig{2};
  auto const ex(orig);
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(*ex == 2);
}

TEST_CASE("trivial copy constructor with error") {
  rd::expected<int, int> const orig{rd::unexpect, 2};
  auto const ex(orig);
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(ex.error() == 2);
}

TEST_CASE("non-trivial copy constructor with value") {
  rd::expected<std::string, int> const orig{"test value"};
  auto const ex(orig);
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(*ex == "test value");
}

TEST_CASE("non-trivial copy constructor with error") {
  rd::expected<int, std::string> const orig{rd::unexpect, "test value"};
  auto const ex(orig);
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(ex.error() == "test value");
}

TEST_CASE(
    "copy constructor with non-trivial error type and non-trival value type "
    "with containing value") {
  rd::expected<std::string, std::string> const orig{"test value"};
  auto const ex(orig);
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(*ex == "test value");
}

TEST_CASE(
    "copy constructor with non-trivial error type and non-trival value type "
    "with containing error") {
  rd::expected<std::string, std::string> const orig{rd::unexpect, "test value"};
  auto const ex(orig);
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig.has_value());
  REQUIRE(ex.error() == "test value");
}

TEST_CASE(
    "move constructor with trivial value and error type with containing "
    "value") {
  rd::expected<int, int> orig{2};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(*ex == 2);
}

TEST_CASE(
    "move constructor with trivial value and error type with containing "
    "error") {
  rd::expected<int, int> orig{rd::unexpect, 2};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(ex.error() == 2);
}

TEST_CASE(
    "move constructor with non-trivial value and trivial error type with "
    "containing value") {
  rd::expected<std::string, int> orig{"test value"};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(*ex == "test value");
}

TEST_CASE(
    "move constructor with trivial value and non-trivial error type with "
    "containing error") {
  rd::expected<int, std::string> orig{rd::unexpect, "test value"};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(ex.error() == "test value");
}

TEST_CASE(
    "move constructor with non trivial value and error type with "
    "containing value") {
  rd::expected<std::string, std::string> orig{"test value"};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(*ex == "test value");
}

TEST_CASE(
    "move constructor with non trivial value and error type with "
    "containing error") {
  rd::expected<std::string, std::string> orig{rd::unexpect, "test value"};
  auto const orig_has_value = orig.has_value();
  auto const ex(std::move(orig));
  REQUIRE(!ex.has_value());
  REQUIRE(ex.has_value() == orig_has_value);
  REQUIRE(ex.error() == "test value");
}

TEST_CASE("expected conversion-copy-constructor with value") {
  rd::expected<int_to_str, int_to_str> orig(2);
  rd::expected<std::string, std::string> ex(orig);
  REQUIRE(ex.has_value());
  REQUIRE(orig.has_value() == ex.has_value());
  REQUIRE(*ex == "2");
}

TEST_CASE("expected conversion-move-constructor with value") {
  rd::expected<int_to_str, int_to_str> orig(2);
  auto const orig_val = orig.has_value();
  rd::expected<std::string, std::string> ex(std::move(orig));
  REQUIRE(ex.has_value());
  REQUIRE(orig_val == ex.has_value());
  REQUIRE(*ex == "2");
}

TEST_CASE("expected conversion-copy-constructor with error") {
  rd::expected<int_to_str, int_to_str> orig(rd::unexpect, 2);
  rd::expected<std::string, std::string> ex(orig);
  REQUIRE(!ex.has_value());
  REQUIRE(orig.has_value() == ex.has_value());
  REQUIRE(ex.error() == "2");
}

TEST_CASE("expected conversion-move-constructor with error") {
  rd::expected<int_to_str, int_to_str> orig(rd::unexpect, 2);
  auto const orig_val = orig.has_value();
  rd::expected<std::string, std::string> ex(std::move(orig));
  REQUIRE(!ex.has_value());
  REQUIRE(orig_val == ex.has_value());
  REQUIRE(ex.error() == "2");
}
