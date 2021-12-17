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

#include <iostream>
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
    CHECK(l == forward_list2<int>());
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

TEST_CASE("assign empty")
{
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(0, 100);

    check_empty_list(l);
}

TEST_CASE("assign count")
{
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(1, 1);

    check_ranged_list(l, 1);
}

TEST_CASE("assign vector")
{
    const std::vector<int> v{1, 2, 3, 4, 5, 6};
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(v.begin(), v.end());

    check_ranged_list(l, 6);
}

TEST_CASE("assign init-list")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5});
    l.assign({ 1, 2, 3 });

    check_ranged_list(l, 3);
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

TEST_CASE("emplace begin")
{
    forward_list2<int> l({ 2, 3, 4, 5});
    l.emplace_after(l.before_begin(), 1);

    check_ranged_list(l, 5);
}

TEST_CASE("emplace middle")
{
    forward_list2<int> l({ 1, 3, 4, 5});
    l.emplace_after(l.begin(), 2);

    check_ranged_list(l, 5);
}

TEST_CASE("emplace end")
{
    forward_list2<int> l({ 1, 2, 3, 4});
    l.emplace_after(l.before_end(), 5);

    check_ranged_list(l, 5);
}

TEST_CASE("erase begin")
{
    forward_list2<int> l({ 7, 1, 2, 3, 4, 5});
    auto it = l.erase_after(l.before_begin());
 
    CHECK(it == l.begin());
    check_ranged_list(l, 5);
}

TEST_CASE("erase middle")
{
    forward_list2<int> l({ 1, 7, 2, 3, 4, 5});
    auto it = l.erase_after(l.begin());

    CHECK(it == std::next(l.begin()));
    check_ranged_list(l, 5);
}

TEST_CASE("erase end")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 8});
    auto it = l.erase_after(std::next(l.before_begin(), 5));

    CHECK(it == l.end());
    check_ranged_list(l, 5);
}

TEST_CASE("erase range begin")
{
    forward_list2<int> l({ 7, 8, 9, 1, 2, 3, 4, 5});
    auto it = l.erase_after(l.before_begin(), std::next(l.before_begin(), 4));
 
    CHECK(it == l.begin());
    check_ranged_list(l, 5);
}

TEST_CASE("erase range middle")
{
    forward_list2<int> l({ 1, 7, 8, 9, 2, 3, 4, 5});
    auto it = l.erase_after(l.begin(), std::next(l.begin(), 4));

    CHECK(it == std::next(l.begin()));
    check_ranged_list(l, 5);
}

TEST_CASE("erase range end")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 7, 8, 9});
    auto it = l.erase_after(std::next(l.before_begin(), 5), l.end());

    CHECK(it == l.end());
    check_ranged_list(l, 5);
}

TEST_CASE("push front and back")
{
    forward_list2<int> l({ 2 });
    l.push_back(3);
    l.push_front(1);

    check_ranged_list(l, 3);
}

TEST_CASE("push front to empty")
{
    forward_list2<int> l;
    l.push_front(1);

    check_ranged_list(l, 1);
}

TEST_CASE("push back to empty")
{
    forward_list2<int> l;
    l.push_back(1);

    check_ranged_list(l, 1);
}

TEST_CASE("emplace front and back")
{
    forward_list2<int> l({ 2 });
    l.emplace_back(3);
    l.emplace_front(1);

    check_ranged_list(l, 3);
}

TEST_CASE("pop front")
{
    forward_list2<int> l({ 0, 1, 2, 3 });
    l.pop_front();

    check_ranged_list(l, 3);
}

TEST_CASE("pop front to empty")
{
    forward_list2<int> l({ 1, 2, 3, 4 });
    while (!l.empty())
        l.pop_front();

    check_empty_list(l);
}

TEST_CASE("resize increase")
{   
    forward_list2<int> l({ 1, 2, 3, 4 });
    l.resize(7);

    CHECK(l == forward_list2<int>{ 1, 2, 3, 4, 0, 0, 0 });

    check_iterators(l);
}

TEST_CASE("resize decrease")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 6, 7 });
    l.resize(3);

    check_ranged_list(l, 3);
}

TEST_CASE("resize value increase")
{   
    forward_list2<int> l({ 1, 2, 3, 4 });
    l.resize(7, 8);

    CHECK(l == forward_list2<int>{ 1, 2, 3, 4, 8, 8, 8 });

    check_iterators(l);
}

TEST_CASE("resize value decrease")
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 6, 7 });
    l.resize(3, 8);

    check_ranged_list(l, 3);
}

TEST_CASE("swap")
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5, 6, 7 });
    forward_list2<int> l2({ 1, 2 });
    l1.swap(l2);

    check_ranged_list(l1, 2);
    check_ranged_list(l2, 7);
}

TEST_CASE("merge")
{
    forward_list2<int> l1({ 1, 3, 5, 6, 7 });
    forward_list2<int> l2({ 2, 4, 8, 9 });

    l1.merge(l2);
    check_ranged_list(l1, 9);
    check_empty_list(l2);
}

TEST_CASE("merge move")
{
    forward_list2<int> l({ 2, 4, 8, 9 });

    l.merge(forward_list2<int>({ 1, 3, 5, 6, 7 }));
    check_ranged_list(l, 9);
}

TEST_CASE("splice whole")
{
    forward_list2<int> l1({ 1, 5, 6, 7 });
    forward_list2<int> l2({ 2, 3, 4 });

    l1.splice_after(l1.begin(), l2);
    check_ranged_list(l1, 7);
    check_empty_list(l2);
}

TEST_CASE("splice whole move")
{
    forward_list2<int> l({ 1, 5, 6, 7 });

    l.splice_after(l.begin(), forward_list2<int>({ 2, 3, 4 }));
    check_ranged_list(l, 7);
}

TEST_CASE("splice one")
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 2, 3, 4 });

    l1.splice_after(l1.begin(), l2, l2.begin());
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_CASE("splice one from end")
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), l2, std::next(l2.begin(), 3));
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_CASE("splice one from end move")
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), std::move(l2), std::next(l2.begin(), 3));
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_CASE("splice")
{
    forward_list2<int> l1({ 1, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), l2, l2.begin(), l2.before_end());
    CHECK(l1 == forward_list2<int>{ 1, 2, 3, 4, 5 });

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 2);
}

TEST_CASE("splice from end")
{
    forward_list2<int> l1({ 1, 5 });
    forward_list2<int> l2({ 1, 2, 2, 3, 4 });

    l1.splice_after(l1.begin(), l2, std::next(l2.begin()), l2.end());
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 2);
}

TEST_CASE("splice to end")
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), l2, l2.begin(), l2.end());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_CASE("splice move")
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), std::move(l2), l2.begin(), l2.end());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_CASE("splice one to self")
{
    forward_list2<int> l({ 1, 5, 2, 3, 4});

    l.splice_after(l.before_end(), l, l.begin());
    check_ranged_list(l, 5);
}

TEST_CASE("splice range to self")
{
    forward_list2<int> l({ 1, 5, 6, 7, 2, 3, 4});

    l.splice_after(l.before_end(), l, l.begin(), std::next(l.begin(), 4));
    check_ranged_list(l, 7);
}
