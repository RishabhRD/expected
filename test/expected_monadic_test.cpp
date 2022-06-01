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

#include <string>

#include "test_include.hpp"

auto to_int(std::string const& str) -> rd::expected<int, std::string> {
  try {
    return std::stoi(str);
  } catch (...) {
    return rd::unexpected{"error"};
  }
}

auto error_to_int(std::string const& str) -> rd::expected<std::string, int> {
  try {
    return rd::unexpected{std::stoi(str)};
  } catch (...) {
    return "0";
  }
}

auto add_1(int x) { return x + 1; }

TEST_CASE("and_then & with value, with success continutation") {
  rd::expected<std::string, std::string> pre{"2"};
  auto post = pre.and_then(to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then & with value, with failed continutation") {
  rd::expected<std::string, std::string> pre{"a2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "error");
}

TEST_CASE("and_then & with error, with success continutation on error value") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "2");
}

TEST_CASE("and_then & with error, with failed continutation on error value") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "a2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "a2");
}

TEST_CASE("and_then const& with value, with success continutation") {
  rd::expected<std::string, std::string> const pre{"2"};
  auto post = pre.and_then(to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then const& with value, with failed continutation") {
  rd::expected<std::string, std::string> const pre{"a2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "error");
}

TEST_CASE(
    "and_then const& with error, with success continutation on error value") {
  rd::expected<std::string, std::string> const pre{rd::unexpect, "2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "2");
}

TEST_CASE(
    "and_then const& with error, with failed continutation on error value") {
  rd::expected<std::string, std::string> const pre{rd::unexpect, "a2"};
  auto post = pre.and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "a2");
}

TEST_CASE("and_then && with value, with success continutation") {
  rd::expected<std::string, std::string> pre{"2"};
  auto post = std::move(pre).and_then(to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then && with value, with failed continutation") {
  rd::expected<std::string, std::string> pre{"a2"};
  auto post = std::move(pre).and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "error");
}

TEST_CASE("and_then && with error, with success continutation on error value") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "2"};
  auto post = std::move(pre).and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "2");
}

TEST_CASE("and_then && with error, with failed continutation on error value") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "a2"};
  auto post = std::move(pre).and_then(to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == "a2");
}

TEST_CASE("or_else & with value, with error continuation success on value") {
  rd::expected<std::string, std::string> pre{"2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "2");
}

TEST_CASE("or_else & with error, with successfull errorcontinuation") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("or_else & with error, with failed error continuation") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "a2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "0");
}

TEST_CASE("or_else & with value, with error continuation failed on value") {
  rd::expected<std::string, std::string> pre{"a2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "a2");
}

TEST_CASE(
    "or_else const& with value, with error continuation success on value") {
  rd::expected<std::string, std::string> const pre{"2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "2");
}

TEST_CASE("or_else const& with error, with successfull errorcontinuation") {
  rd::expected<std::string, std::string> const pre{rd::unexpect, "2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("or_else const& with error, with failed error continuation") {
  rd::expected<std::string, std::string> const pre{rd::unexpect, "a2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "0");
}

TEST_CASE(
    "or_else const& with value, with error continuation failed on value") {
  rd::expected<std::string, std::string> const pre{"a2"};
  auto post = pre.or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "a2");
}

TEST_CASE("or_else && with value, with error continuation success on value") {
  rd::expected<std::string, std::string> pre{"2"};
  auto post = std::move(pre).or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "2");
}

TEST_CASE("or_else && with error, with successfull errorcontinuation") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "2"};
  auto post = std::move(pre).or_else(error_to_int);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("or_else && with error, with failed error continuation") {
  rd::expected<std::string, std::string> pre{rd::unexpect, "a2"};
  auto post = std::move(pre).or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "0");
}

TEST_CASE("or_else && with value, with error continuation failed on value") {
  rd::expected<std::string, std::string> pre{"a2"};
  auto post = std::move(pre).or_else(error_to_int);
  REQUIRE(post.has_value());
  REQUIRE(*post == "a2");
}

TEST_CASE("transform & with value") {
  rd::expected<int, int> pre{2};
  auto post = pre.transform(add_1);
  REQUIRE(post.has_value());
  REQUIRE(*post == 3);
}

TEST_CASE("transform & with error") {
  rd::expected<int, int> pre{rd::unexpect, 2};
  auto post = pre.transform(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform const& with value") {
  rd::expected<int, int> const pre{2};
  auto post = pre.transform(add_1);
  REQUIRE(post.has_value());
  REQUIRE(*post == 3);
}

TEST_CASE("transform const& with error") {
  rd::expected<int, int> const pre{rd::unexpect, 2};
  auto post = pre.transform(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform && with value") {
  rd::expected<int, int> pre{2};
  auto post = std::move(pre).transform(add_1);
  REQUIRE(post.has_value());
  REQUIRE(*post == 3);
}

TEST_CASE("transform && with error") {
  rd::expected<int, int> pre{rd::unexpect, 2};
  auto post = std::move(pre).transform(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform_error & with value") {
  rd::expected<int, int> pre{2};
  auto post = pre.transform_error(add_1);
  REQUIRE(post.has_value());
  REQUIRE(post.value() == 2);
}

TEST_CASE("transform_error & with error") {
  rd::expected<int, int> pre{rd::unexpect, 2};
  auto post = pre.transform_error(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("transform_error const& with value") {
  rd::expected<int, int> const pre{2};
  auto post = pre.transform_error(add_1);
  REQUIRE(post.has_value());
  REQUIRE(post.value() == 2);
}

TEST_CASE("transform_error const& with error") {
  rd::expected<int, int> const pre{rd::unexpect, 2};
  auto post = pre.transform_error(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("transform_error && with value") {
  rd::expected<int, int> pre{2};
  auto post = std::move(pre).transform_error(add_1);
  REQUIRE(post.has_value());
  REQUIRE(post.value() == 2);
}

TEST_CASE("transform_error && with error") {
  rd::expected<int, int> pre{rd::unexpect, 2};
  auto post = std::move(pre).transform_error(add_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}
