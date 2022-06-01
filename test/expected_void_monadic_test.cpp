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

auto create_expected_success() { return rd::expected<int, int>(2); }
auto create_expected_error() { return rd::expected<int, int>(rd::unexpect, 2); }

auto create_void_expected_success(int /*unused*/) {
  return rd::expected<void, int>{};
}
auto create_void_expected_error(int /*unused*/) {
  return rd::expected<void, int>{rd::unexpect, 2};
}

auto sum_1(int n) { return n + 1; }

auto get_num() { return 1; }

TEST_CASE("and_then & with value continuation with value") {
  rd::expected<void, int> pre;
  auto post = pre.and_then(create_expected_success);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then & with value continuation with error") {
  rd::expected<void, int> pre;
  auto post = pre.and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("and_then & with error continuation with value") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = pre.and_then(create_expected_success);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("and_then & with error continuation with failure") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = pre.and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("and_then const& with value continuation with value") {
  rd::expected<void, int> const pre;
  auto post = pre.and_then(create_expected_success);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then const& with value continuation with error") {
  rd::expected<void, int> const pre;
  auto post = pre.and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("and_then const& with error continuation with value") {
  rd::expected<void, int> const pre{rd::unexpect, 3};
  auto post = pre.and_then(create_expected_success);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("and_then const& with error continuation with failure") {
  rd::expected<void, int> const pre{rd::unexpect, 3};
  auto post = pre.and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("and_then && with value continuation with value") {
  rd::expected<void, int> pre;
  auto post = std::move(pre).and_then(create_expected_success);
  REQUIRE(post.has_value());
  REQUIRE(*post == 2);
}

TEST_CASE("and_then && with value continuation with error") {
  rd::expected<void, int> pre;
  auto post = std::move(pre).and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("and_then && with error continuation with value") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = std::move(pre).and_then(create_expected_success);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("and_then && with error continuation with failure") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = std::move(pre).and_then(create_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("or_else & with value, continuation with value") {
  rd::expected<void, int> pre;
  auto post = pre.or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else & with value, continuation with error") {
  rd::expected<void, int> pre;
  auto post = pre.or_else(create_void_expected_error);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else & with error, continuation with value") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = pre.or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else & with error, continuation with error") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = pre.or_else(create_void_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("or_else const& with value, continuation with value") {
  rd::expected<void, int> const pre;
  auto post = pre.or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else const& with value, continuation with error") {
  rd::expected<void, int> const pre;
  auto post = pre.or_else(create_void_expected_error);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else const& with error, continuation with value") {
  rd::expected<void, int> const pre{rd::unexpect, 3};
  auto post = pre.or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else const& with error, continuation with error") {
  rd::expected<void, int> const pre{rd::unexpect, 3};
  auto post = pre.or_else(create_void_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("or_else && with value, continuation with value") {
  rd::expected<void, int> pre;
  auto post = std::move(pre).or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else && with value, continuation with error") {
  rd::expected<void, int> pre;
  auto post = std::move(pre).or_else(create_void_expected_error);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else && with error, continuation with value") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = std::move(pre).or_else(create_void_expected_success);
  REQUIRE(post.has_value());
}

TEST_CASE("or_else && with error, continuation with error") {
  rd::expected<void, int> pre{rd::unexpect, 3};
  auto post = std::move(pre).or_else(create_void_expected_error);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform & with value") {
  rd::expected<void, int> pre;
  auto post = pre.transform(get_num);
  REQUIRE(post.has_value());
  REQUIRE(*post == 1);
}

TEST_CASE("transform & with error") {
  rd::expected<void, int> pre{rd::unexpect, 2};
  auto post = pre.transform(get_num);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform const& with value") {
  rd::expected<void, int> const pre;
  auto post = pre.transform(get_num);
  REQUIRE(post.has_value());
  REQUIRE(*post == 1);
}

TEST_CASE("transform const& with error") {
  rd::expected<void, int> const pre{rd::unexpect, 2};
  auto post = pre.transform(get_num);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform && with value") {
  rd::expected<void, int> pre;
  auto post = std::move(pre).transform(get_num);
  REQUIRE(post.has_value());
  REQUIRE(*post == 1);
}

TEST_CASE("transform && with error") {
  rd::expected<void, int> pre{rd::unexpect, 2};
  auto post = std::move(pre).transform(get_num);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 2);
}

TEST_CASE("transform_error & with value") {
  rd::expected<void, int> pre{};
  auto post = pre.transform_error(sum_1);
  REQUIRE(post.has_value());
}

TEST_CASE("transform_error & with error") {
  rd::expected<void, int> pre{rd::unexpect, 2};
  auto post = pre.transform_error(sum_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("transform_error const& with value") {
  rd::expected<void, int> const pre{};
  auto post = pre.transform_error(sum_1);
  REQUIRE(post.has_value());
}

TEST_CASE("transform_error const& with error") {
  rd::expected<void, int> const pre{rd::unexpect, 2};
  auto post = pre.transform_error(sum_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}

TEST_CASE("transform_error && with value") {
  rd::expected<void, int> pre{};
  auto post = std::move(pre).transform_error(sum_1);
  REQUIRE(post.has_value());
}

TEST_CASE("transform_error && with error") {
  rd::expected<void, int> pre{rd::unexpect, 2};
  auto post = std::move(pre).transform_error(sum_1);
  REQUIRE(!post.has_value());
  REQUIRE(post.error() == 3);
}
