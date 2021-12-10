/*
 * Copyright (c) 2021 Pavel I. Kryukov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../forward_list2.hpp"

#include "catch.hpp"

#include <utility>

static_assert(sizeof(forward_list2<int>) == 2 * sizeof(void*));

static void check_iterators(forward_list2<int>& l)
{
    CHECK(std::next(l.before_begin()) == l.begin());
    CHECK(std::next(l.before_end()) == l.end());
    CHECK(std::next(l.cbefore_begin()) == l.cbegin());
    CHECK(std::next(l.cbefore_end()) == l.cend());
    CHECK(std::next(std::as_const(l).before_begin()) == std::as_const(l).begin());
    CHECK(std::next(std::as_const(l).before_end()) == std::as_const(l).end());
}

static void check_empty_list(forward_list2<int>& l)
{
    CHECK(l.empty());
    CHECK(l.begin() == l.end());
    CHECK(l.before_begin() == l.before_end());
    check_iterators(l);
}

static void check_ranged_list(forward_list2<int>& l, size_t range)
{
    int count = 0;
    for (auto i : l) {
        ++count;
        CHECK(i == count);
    }

    CHECK(count == range);
    CHECK(!l.empty());
    CHECK(l.front() == 1);
    CHECK(l.back() == range);
    CHECK(std::as_const(l).front() == 1);
    CHECK(std::as_const(l).back() == range);
    check_iterators(l);
}

TEST_CASE("empty list")
{
    forward_list2<int> l;

    check_empty_list(l);
}

TEST_CASE("counter-init list")
{
    forward_list2<int> l(10, 20);

    int count = 0;
    for (auto i : l) {
        CHECK(i == 20);
        ++count;
    }

    CHECK(!l.empty());
    CHECK(count == 10);
    CHECK(l.front() == 20);
    CHECK(l.back() == 20);
    CHECK(std::as_const(l).front() == 20);
    CHECK(std::as_const(l).back() == 20);
    check_iterators(l);
}

TEST_CASE("iterator-init list")
{
    const std::vector<int> v{ 1, 2, 3, 4, 5 };
    forward_list2<int> l(v.begin(), v.end());

    check_ranged_list(l, 5);
}

TEST_CASE("init-list list")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5 });

    check_ranged_list(l, 5);
}


TEST_CASE("list copy")
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = l1;

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 5);
}

TEST_CASE("list clear")
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = l1;
    l1.clear();

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_CASE("list move")
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = std::move(l1);

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_CASE("insert back")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5 });
    l.insert_after(l.before_end(), 6);

    check_ranged_list(l, 6);
}

TEST_CASE("insert front")
{
    forward_list2<int> l({ 2, 3, 4, 5, 6 });
    l.insert_after(l.before_begin(), 1);

    check_ranged_list(l, 6);
}

TEST_CASE("insert middle")
{
    forward_list2<int> l({ 1, 3, 4, 5, 6 });
    l.insert_after(l.begin(), 2);

    check_ranged_list(l, 6);
}

TEST_CASE("insert iterators")
{
    forward_list2<int> l({ 1, 6 });
    std::vector<int> v{2, 3, 4, 5};
    l.insert_after(l.begin(), v.begin(), v.end());

    check_ranged_list(l, 6);
}

TEST_CASE("insert iterators to end")
{
    forward_list2<int> l({ 1 });
    std::vector<int> v{2, 3, 4, 5};
    l.insert_after(l.begin(), v.begin(), v.end());

    check_ranged_list(l, 5);
}

TEST_CASE("insert nothing")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5});
    l.insert_after(l.begin(), 0, 100);

    check_ranged_list(l, 5);
}
