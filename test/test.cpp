/*
 * Copyright (c) 2021-2022 Pavel I. Kryukov
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

#include <gtest/gtest.h>

#include <utility>

#ifdef __cpp_lib_as_const

using std::as_const;

#else

template <class T>
constexpr typename std::add_const<T>::type& as_const(T& t) noexcept
{
    return t;
}

#endif

static_assert(sizeof(forward_list2<int>) == 2 * sizeof(void*), "forward_list2 must be 2 pointers");

class ForwardList : public ::testing::Test
{
protected:
    void check_iterators(forward_list2<int>& l)
    {
        EXPECT_TRUE(l.max_size() > 0);
        EXPECT_TRUE(std::distance(l.begin(), l.end()) < l.max_size());
        EXPECT_EQ(std::distance(l.cbefore_begin(), l.cbefore_end()), std::distance(l.begin(), l.end()));
        EXPECT_EQ(std::next(l.before_begin()), l.begin());
        EXPECT_EQ(std::next(l.before_end()), l.end());
        EXPECT_EQ(std::next(l.cbefore_begin()), l.cbegin());
        EXPECT_EQ(std::next(l.cbefore_end()), l.cend());
        EXPECT_EQ(std::next(as_const(l).before_begin()), as_const(l).begin());
        EXPECT_EQ(std::next(as_const(l).before_end()), as_const(l).end());
    }

    void check_empty_list(forward_list2<int>& l)
    {
        EXPECT_EQ(l, forward_list2<int>());
        EXPECT_TRUE(l.empty());
        EXPECT_EQ(l.begin(), l.end());
        EXPECT_EQ(l.before_begin(), l.before_end());
        check_iterators(l);
    }

    void check_ranged_list(forward_list2<int>& l, size_t range)
    {
        int count = 0;
        for (auto i : l) {
            ++count;
            EXPECT_EQ(i, count);
        }

        EXPECT_NE(l, forward_list2<int>{});
        EXPECT_EQ(count, range);
        EXPECT_FALSE(l.empty());
        EXPECT_EQ(l.front(), 1);
        EXPECT_EQ(l.back(), range);
        EXPECT_EQ(as_const(l).front(), 1);
        EXPECT_EQ(as_const(l).back(), range);
        check_iterators(l);
    }
};

TEST_F(ForwardList, Empty)
{
    forward_list2<int> l;

    check_empty_list(l);
}

TEST_F(ForwardList, CounterInit)
{
    const int value = 20;
    const size_t size = 10;
    forward_list2<int> l(size, value);

    int count = 0;
    for (auto i : l) {
        EXPECT_EQ(i, value);
        ++count;
    }

    EXPECT_TRUE(!l.empty());
    EXPECT_EQ(count, size);
    EXPECT_EQ(l.front(), value);
    EXPECT_EQ(l.back(), value);
    EXPECT_EQ(as_const(l).front(), value);
    EXPECT_EQ(as_const(l).back(), value);
    check_iterators(l);
}

TEST_F(ForwardList, IteratorInit)
{
    const std::vector<int> v{ 1, 2, 3, 4, 5 };
    forward_list2<int> l(v.begin(), v.end());

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, ListInit)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5 });

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, Copy)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = l1;

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, CopyWithAllocator)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    forward_list2<int> l2(l1, std::allocator<int>());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, Clear)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = l1;
    l1.clear();

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, Move)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    auto l2 = std::move(l1);

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, MoveAllocator)
{
    forward_list2<int> l1{ 1, 2, 3, 4, 5 };
    forward_list2<int> l2(std::move(l1), std::allocator<int>());

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, CopyAssign)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    forward_list2<int> l2{3, 4, 8, 11};
    l2 = l1;

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, MoveAssign)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5 });
    forward_list2<int> l2{3, 4, 8, 11};
    l2 = std::move(l1);

    check_empty_list(l1);
    check_ranged_list(l2, 5);
}

TEST_F(ForwardList, AssignOperatorInitialization)
{
    forward_list2<int> l{3, 4, 8, 11};
    l = { 1, 2, 3, 4, 5 };

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, AssignEmpty)
{
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(0, 100);

    check_empty_list(l);
}

TEST_F(ForwardList, AssignCount)
{
    const int value = 1;
    const size_t size = 1;
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(size, value);

    check_ranged_list(l, 1);
}

TEST_F(ForwardList, AssignVector)
{
    const std::vector<int> v{1, 2, 3, 4, 5, 6};
    forward_list2<int> l({ 2, 3, 1, 8 });
    l.assign(v.begin(), v.end());

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, AssignInitializationList)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5});
    l.assign({ 1, 2, 3 });

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, InsertBack)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5 });
    l.insert_after(l.before_end(), 6);

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, InsertBackCopy)
{
    const int six = 6;
    forward_list2<int> l({ 1, 2, 3, 4, 5 });
    l.insert_after(l.before_end(), six);

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, InsertFront)
{
    forward_list2<int> l({ 2, 3, 4, 5, 6 });
    l.insert_after(l.before_begin(), 1);

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, InsertMiddle)
{
    forward_list2<int> l({ 1, 3, 4, 5, 6 });
    l.insert_after(l.begin(), 2);

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, InsertIterators)
{
    forward_list2<int> l({ 1, 6 });
    std::vector<int> v{2, 3, 4, 5};
    l.insert_after(l.begin(), v.begin(), v.end());

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, InsertIteratorsToEnd)
{
    forward_list2<int> l({ 1 });
    std::vector<int> v{2, 3, 4, 5};
    l.insert_after(l.begin(), v.begin(), v.end());

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, InsertNothing)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5});
    l.insert_after(l.begin(), 0, 100);

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EmplaceBegin)
{
    forward_list2<int> l({ 2, 3, 4, 5});
    l.emplace_after(l.before_begin(), 1);

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EmplaceMiddle)
{
    forward_list2<int> l({ 1, 3, 4, 5});
    l.emplace_after(l.begin(), 2);

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EmplaceEnd)
{
    forward_list2<int> l({ 1, 2, 3, 4});
    l.emplace_after(l.before_end(), 5);

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseBegin)
{
    forward_list2<int> l({ 7, 1, 2, 3, 4, 5});
    auto it = l.erase_after(l.before_begin());
 
    EXPECT_EQ(it, l.begin());
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseMiddle)
{
    forward_list2<int> l({ 1, 7, 2, 3, 4, 5});
    auto it = l.erase_after(l.begin());

    EXPECT_EQ(it, std::next(l.begin()));
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseEnd)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 8});
    auto it = l.erase_after(std::next(l.before_begin(), 5));

    EXPECT_EQ(it, l.end());
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseRangeBegin)
{
    forward_list2<int> l({ 7, 8, 9, 1, 2, 3, 4, 5});
    auto it = l.erase_after(l.before_begin(), std::next(l.before_begin(), 4));
 
    EXPECT_EQ(it, l.begin());
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseRangeMiddle)
{
    forward_list2<int> l({ 1, 7, 8, 9, 2, 3, 4, 5});
    auto it = l.erase_after(l.begin(), std::next(l.begin(), 4));

    EXPECT_EQ(it, std::next(l.begin()));
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, EraseRangeEnd)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 7, 8, 9});
    auto it = l.erase_after(std::next(l.before_begin(), 5), l.end());

    EXPECT_EQ(it, l.end());
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, PushFrontAndBack)
{
    forward_list2<int> l({ 2 });
    l.push_back(3);
    l.push_front(1);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, PushFrontToEmpty)
{
    forward_list2<int> l;
    l.push_front(1);

    check_ranged_list(l, 1);
}

TEST_F(ForwardList, PushBackToEmpty)
{
    forward_list2<int> l;
    l.push_back(1);

    check_ranged_list(l, 1);
}

TEST_F(ForwardList, PushCopy)
{
    int three = 3;
    int one = 1;
    forward_list2<int> l({ 2 });
    l.push_back(three);
    l.push_front(one);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, EmplaceFrontAndBack)
{
    forward_list2<int> l({ 2 });
    const auto& three = l.emplace_back(3);
    const auto& one = l.emplace_front(1);

    EXPECT_EQ(three, 3);
    EXPECT_EQ(one, 1);
    check_ranged_list(l, 3);
}

TEST_F(ForwardList, PopFront)
{
    forward_list2<int> l({ 0, 1, 2, 3 });
    l.pop_front();

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, PopFrontToEmpty)
{
    forward_list2<int> l({ 1, 2, 3, 4 });
    while (!l.empty())
        l.pop_front();

    check_empty_list(l);
}

TEST_F(ForwardList, ResizeIncrease)
{   
    forward_list2<int> l({ 1, 2, 3, 4 });
    l.resize(7);

    EXPECT_EQ(l, (forward_list2<int>{ 1, 2, 3, 4, 0, 0, 0 }));

    check_iterators(l);
}

TEST_F(ForwardList, ResizeDecrease)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 6, 7 });
    l.resize(3);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, ResizeValueIncrease)
{   
    forward_list2<int> l({ 1, 2, 3, 4 });
    l.resize(7, 8);

    EXPECT_EQ(l, (forward_list2<int>{ 1, 2, 3, 4, 8, 8, 8 }));

    check_iterators(l);
}

TEST_F(ForwardList, ResizeValueDecrease)
{
    forward_list2<int> l({ 1, 2, 3, 4, 5, 6, 7 });
    l.resize(3, 8);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, Swap)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5, 6, 7 });
    forward_list2<int> l2({ 1, 2 });
    l1.swap(l2);

    check_ranged_list(l1, 2);
    check_ranged_list(l2, 7);
}

TEST_F(ForwardList, StdSwap)
{
    forward_list2<int> l1({ 1, 2, 3, 4, 5, 6, 7 });
    forward_list2<int> l2({ 1, 2 });
    std::swap(l2, l1);

    check_ranged_list(l1, 2);
    check_ranged_list(l2, 7);
}

TEST_F(ForwardList, Merge)
{
    forward_list2<int> l1({ 1, 3, 5, 6, 7 });
    forward_list2<int> l2({ 2, 4, 8, 9 });

    l1.merge(l2);
    check_ranged_list(l1, 9);
    check_empty_list(l2);
}

TEST_F(ForwardList, MergeMove)
{
    forward_list2<int> l({ 2, 4, 8, 9 });

    l.merge(forward_list2<int>({ 1, 3, 5, 6, 7 }));
    check_ranged_list(l, 9);
}

TEST_F(ForwardList, MergeCompare)
{
    forward_list2<int> l1({ 1, 3, 5, 6, 7 });
    forward_list2<int> l2({ 2, 4, 8, 9 });
    l1.reverse();
    l2.reverse();

    l1.merge(l2, [](int x, int y){ return x > y; });
    l1.reverse();

    check_ranged_list(l1, 9);
    check_empty_list(l2);
}

TEST_F(ForwardList, MergeMoveCompare)
{
    forward_list2<int> l({ 1, 3, 5, 6, 7 });
    l.reverse();

    l.merge(forward_list2<int>{ 9, 8, 4, 2 }, [](int x, int y){ return x > y; });
    l.reverse();

    check_ranged_list(l, 9);
}

TEST_F(ForwardList, SpliceWhole)
{
    forward_list2<int> l1({ 1, 5, 6, 7 });
    forward_list2<int> l2({ 2, 3, 4 });

    l1.splice_after(l1.begin(), l2);
    check_ranged_list(l1, 7);
    check_empty_list(l2);
}

TEST_F(ForwardList, SpliceWholeMove)
{
    forward_list2<int> l({ 1, 5, 6, 7 });

    l.splice_after(l.begin(), forward_list2<int>({ 2, 3, 4 }));
    check_ranged_list(l, 7);
}

TEST_F(ForwardList, SpliceOne)
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 2, 3, 4 });

    l1.splice_after(l1.begin(), l2, l2.begin());
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_F(ForwardList, SpliceOneFromEnd)
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), l2, std::next(l2.begin(), 3));
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_F(ForwardList, SpliceOneFromEndMove)
{
    forward_list2<int> l1({ 1, 3, 4, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), std::move(l2), std::next(l2.begin(), 3));
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 4);
}

TEST_F(ForwardList, SpliceOneToEnd)
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), l2, l2.begin());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_F(ForwardList, SpliceOneToEndMove)
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), std::move(l2), l2.begin());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_F(ForwardList, Splice)
{
    forward_list2<int> l1({ 1, 5 });
    forward_list2<int> l2({ 1, 2, 3, 4, 2 });

    l1.splice_after(l1.begin(), l2, l2.begin(), l2.before_end());
    EXPECT_EQ(l1, (forward_list2<int>{ 1, 2, 3, 4, 5 }));

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 2);
}

TEST_F(ForwardList, SpliceFromEnd)
{
    forward_list2<int> l1({ 1, 5 });
    forward_list2<int> l2({ 1, 2, 2, 3, 4 });

    l1.splice_after(l1.begin(), l2, std::next(l2.begin()), l2.end());
    check_ranged_list(l1, 5);
    check_ranged_list(l2, 2);
}

TEST_F(ForwardList, SpliceToEnd)
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), l2, l2.begin(), l2.end());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_F(ForwardList, SpliceMove)
{
    forward_list2<int> l1{ 1, 2, 3, 4 };
    forward_list2<int> l2{ 1, 5 };

    l1.splice_after(l1.before_end(), std::move(l2), l2.begin(), l2.end());

    check_ranged_list(l1, 5);
    check_ranged_list(l2, 1);
}

TEST_F(ForwardList, SpliceOneToSelf)
{
    forward_list2<int> l({ 1, 5, 2, 3, 4});

    l.splice_after(l.before_end(), l, l.begin());
    check_ranged_list(l, 5);
}

TEST_F(ForwardList, SpliceRangeToSelf)
{
    forward_list2<int> l({ 1, 5, 6, 7, 2, 3, 4});

    l.splice_after(l.before_end(), l, l.begin(), std::next(l.begin(), 4));
    check_ranged_list(l, 7);
}

TEST_F(ForwardList, Remove)
{
    forward_list2<int> l({ 1, 0, 2, 0, 3, 0});
    l.remove(0);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, RemovePredicate)
{
    forward_list2<int> l({ 1, -4, 2, -5, 3, -6});
    l.remove_if([](int x){ return x < 0; });

    check_ranged_list(l, 3);
}

#ifdef __cpp_lib_erase_if
TEST_F(ForwardList, StdErase)
{
    forward_list2<int> l({ 1, 0, 2, 0, 3, 0});
    std::erase(l, 0);

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, StdEraseIf)
{
    forward_list2<int> l({ 1, -4, 2, -5, 3, -6});
    std::erase_if(l, [](int x){ return x < 0; });

    check_ranged_list(l, 3);
}
#endif

TEST_F(ForwardList, Reverse)
{
    forward_list2<int> l{ 5, 4, 3, 2, 1};
    l.reverse();

    check_ranged_list(l, 5);
}

TEST_F(ForwardList, Unique)
{
    forward_list2<int> l{ 1, 1, 1, 2, 3, 3, 3, 4, 4, 4, 4};
    l.unique();

    check_ranged_list(l, 4);
}

TEST_F(ForwardList, UniquePredicate)
{
    forward_list2<int> l({ 1, -1, 2, -2, 3, -3});
    l.unique([](int x, int y){ return std::abs(x) == std::abs(y); });

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, Sort)
{
    forward_list2<int> l({ 5, 6, 1, 3, 2, 4 });
    l.sort();

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, SortReverse)
{
    forward_list2<int> l({ 5, 6, 1, 3, 2, 4 });
    l.sort([](int x, int y){ return x > y; });
    l.reverse();

    check_ranged_list(l, 6);
}

TEST_F(ForwardList, Spaceship)
{
    forward_list2<int> a{1, 2, 3};
    forward_list2<int> b{4, 5, 6};

    EXPECT_FALSE(a == b);
    EXPECT_NE(a, b);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(a >= b);

    EXPECT_EQ(a, a);
    EXPECT_FALSE(a != a);
    EXPECT_FALSE(a < a);
    EXPECT_FALSE(a > a);
    EXPECT_TRUE(a >= a);
    EXPECT_TRUE(a <= a);
}

TEST_F(ForwardList, MergeEmpty)
{
    forward_list2<int> l1, l2;
    l1.merge(l2);

    check_empty_list(l1);
    check_empty_list(l2);
}

TEST_F(ForwardList, MergeToEmpty)
{
    forward_list2<int> l;
    l.merge(forward_list2<int>{1, 2, 3});

    check_ranged_list(l, 3);
}

TEST_F(ForwardList, MergeToSelf)
{
    forward_list2<int> l{1, 2, 3, 4};
    l.merge(l);

    check_ranged_list(l, 4);
}

TEST_F(ForwardList, SpliceEndToEnd)
{
    forward_list2<int> l{1, 2, 3, 4};
    l.splice_after(std::next(l.begin(), 2), l, std::next(l.begin(), 2));

    check_ranged_list(l, 4);
}

TEST_F(ForwardList, SpliceSamePlace)
{
    forward_list2<int> l{1, 2, 3, 4};
    l.splice_after(std::next(l.begin(), 2), l, std::next(l.begin(), 1));

    check_ranged_list(l, 4);
}

TEST_F(ForwardList, SpliceEmptyToEmpty)
{
    forward_list2<int> l;
    l.splice_after(l.before_begin(), l, l.before_begin());

    check_empty_list(l);
}
