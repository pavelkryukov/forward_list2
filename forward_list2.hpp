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
 
#ifndef FORWARD_LIST_2_HPP
#define FORWARD_LIST_2_HPP
 
#include <forward_list>

template<typename T, class Allocator = std::allocator<T>>
class forward_list2
{
    using Base = std::forward_list<T, Allocator>;
public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using size_type       = typename Base::size_type;
    using difference_type = typename Base::difference_type;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer   = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator        = typename Base::iterator;
    using const_iterator  = typename Base::const_iterator;

    forward_list2() :
        m_list()
    {
        adjust_last_iterator_on_clear();
    }

    explicit forward_list2(const Allocator& alloc) :
        m_list(alloc)
    {
        adjust_last_iterator_on_clear();
    }

    forward_list2(size_type count, const T& value, const Allocator& alloc = Allocator()) :
        forward_list2(alloc)
    {
        insert_to_empty(count, value);
    }

    template<class InputIt>
    forward_list2(InputIt first, InputIt last, const Allocator& alloc = Allocator()) :
        forward_list2(alloc)
    {
        insert_to_empty(first, last);
    }

    forward_list2(const forward_list2& other) : 
        forward_list2(std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator()))
    {
        insert_to_empty(other.begin(), other.end());
    }

    forward_list2(const forward_list2& other, const Allocator& alloc) : 
        forward_list2(alloc)
    {
        insert_to_empty(other.begin(), other.end());
    }

    forward_list2(forward_list2&& other) :
        m_list(std::move(other.m_list)), m_last(std::move(other.m_last))
    {
        other.adjust_last_iterator_on_clear();
    }

    forward_list2(forward_list2&& other, const Allocator& alloc) :
        forward_list2(alloc)
    {
        m_last = m_list.insert_after(before_begin(),
            std::make_move_iterator(other.begin()),
            std::make_move_iterator(other.end()));
        other.clear();
    }

    forward_list2(std::initializer_list<T> init, const Allocator& alloc = Allocator()) :
        forward_list2(alloc)
    {
        insert_to_empty(init);
    }

    ~forward_list2() = default;

    void assign(size_t count, const T& value)
    {
        m_list.clear();
        insert_to_empty(count, value);
    }

    template<class InputIt>
    void assign(InputIt first, InputIt last)
    {
        m_list.clear();
        insert_to_empty(first, last);
    }

    void assign(std::initializer_list<T> ilist )
    {
        m_list.clear();
        insert_to_empty(ilist);
    }

    allocator_type get_allocator() const noexcept { return m_list.get_allocator(); }

    reference front()             { return *begin(); }
    const_reference front() const { return *begin(); }

    // New!
//    reference back()             { return *before_end(); }
    const_reference back() const { return *before_end(); }

    iterator before_begin()              noexcept { return m_list.before_begin(); }
    const_iterator before_begin()  const noexcept { return m_list.before_begin(); }
    const_iterator cbefore_begin() const noexcept { return m_list.cbefore_begin(); }

    iterator begin()              noexcept { return m_list.begin(); }
    const_iterator begin()  const noexcept { return m_list.begin(); }
    const_iterator cbegin() const noexcept { return m_list.cbegin(); }

    // New!
//    iterator before_end()              noexcept { return m_last; }
    const_iterator before_end()  const noexcept { return m_last; }
    const_iterator cbefore_end() const noexcept { return m_last; }

    iterator end()              noexcept { return m_list.end(); }
    const_iterator end()  const noexcept { return m_list.end(); }
    const_iterator cend() const noexcept { return m_list.cend(); }

    bool empty() const noexcept { return m_list.empty(); }
    size_type max_size() const noexcept { return m_list.last_size(); }

    void clear() noexcept
    {
        m_list.clear();
        adjust_last_iterator_on_clear();
    }

    iterator insert_after(const_iterator pos, const T& value )
    {
        auto last_pos = m_list.insert_after(pos, value);
        adjust_last_iterator_on_insertion(pos, last_pos);
        return last_pos;
    }

    iterator insert_after(const_iterator pos, size_type count, const T& value)
    {
        auto last_pos = m_list.insert_after(pos, count, value);
        adjust_last_iterator_on_insertion(pos, last_pos);
        return last_pos;
    }

    template<class InputIt>
    iterator insert_after(const_iterator pos, InputIt first, InputIt last)
    {
        auto last_pos = m_list.insert_after(pos, first, last);
        adjust_last_iterator_on_insertion(pos, last_pos);
        return last_pos;
    }

    template<class... Args>
    iterator emplace_after(const_iterator pos, Args&&... args)
    {
        auto last_pos = m_list.emplace_after(pos, std::forward<Args>(args)...);
        adjust_last_iterator_on_insertion(pos, last_pos);
        return last_pos;
    }

    iterator erase_after(const_iterator pos)
    {
        auto next = m_list.erase_after(pos);
        adjust_last_iterator_on_deletion(pos, next);
        return next;
    }
    
    iterator erase_after(const_iterator first, const_iterator last)
    {
        auto next = m_list.erase_after(first, last);
        adjust_last_iterator_on_deletion(first, last);
        return next;
    }

    void push_front(const T& value) { insert_after(before_begin(), value); }
    void push_front(T&& value)      { insert_after(before_begin(), std::move(value)); }

    // New!
    void push_back(const T& value)  { insert_after(before_end(), value); }
    void push_back(T&& value)       { insert_after(before_end(), std::move(value)); }
    
    template<class... Args>
    void emplace_front(Args&&... args) { emplace_after(before_begin(), std::forward<Args>(args)...); }

    // New!
    template<class... Args>
    void emplace_back(Args&&... args) { emplace_after(before_end(), std::forward<Args>(args)...); }

    void pop_front() { erase_after(before_begin()); }
    
    void resize(size_type count, const T& value)
    {
        auto it = before_begin();
        for (size_type i = 0; i < count; ++i, ++it) {
            if (it == before_end()) {
                insert_after(it, count - i, value);
                return;
            }
        }

        erase_after(it, end());
    }

    void resize(size_type count)
    {
        resize(count, T{});
    }

    void swap(forward_list2& other) noexcept
    {
        std::swap(m_list, other.m_list);
        std::swap(m_last, other.m_last);
    }

    void merge(forward_list2& other)
    {
        m_list.merge(other.m_list);
        adjust_last_iterator_on_merge(other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    void merge(forward_list2&& other)
    {
        m_list.merge(std::move(other.m_list));
        adjust_last_iterator_on_merge(other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    template <class Compare>
    void merge(forward_list2& other, Compare comp)
    {
        m_list.merge(other.m_list, comp);
        adjust_last_iterator_on_merge(other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    template <class Compare>
    void merge(forward_list2&& other, Compare comp)
    {
        m_list.merge(std::move(other.m_list), comp);
        adjust_last_iterator_on_merge(other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    void splice_after(const_iterator pos, forward_list2& other)
    {
        m_list.splice_after(pos, std::move(other.m_list));
        adjust_last_iterator_on_insertion(pos, other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    void splice_after(const_iterator pos, forward_list2&& other)
    {
        m_list.splice_after(pos, std::move(other.m_list));
        adjust_last_iterator_on_insertion(pos, other.m_last);
        other.adjust_last_iterator_on_clear();
    }

    void splice_after(const_iterator pos, forward_list2& other, const_iterator it)
    {
        if (pos == m_last && it == other.m_last)
            m_last = it;
        else
            adjust_last_iterator_on_insertion(pos, std::next(it));

        m_list.splice_after(pos, other.m_list, it);
        other.adjust_last_iterator_on_deletion(it, std::next(it));
    }

    void splice_after(const_iterator pos, forward_list2&& other, const_iterator it)
    {
        if (pos == m_last && it == other.m_last)
            m_last = it;
        else
            adjust_last_iterator_on_insertion(pos, it);

        m_list.splice_after(pos, std::move(other.m_list), it);
        other.adjust_last_iterator_on_deletion(it, std::next(it));
    }

    void splice_after(const_iterator pos, forward_list2& other, const_iterator first, const_iterator last)
    {
        if (pos == m_last && last == other.end())
            m_last = other.m_last;
        else
            adjust_last_iterator_on_insertion(pos, last);

        m_list.splice_after(pos, other.m_list, first, last);
        other.adjust_last_iterator_on_deletion(first, last);
    }

    void splice_after(const_iterator pos, forward_list2&& other, const_iterator first, const_iterator last)
    {
        if (pos == m_last && last == other.end())
            m_last = other.m_last;
        else
            adjust_last_iterator_on_insertion(pos, last);

        m_list.splice_after(pos, std::move(other.m_list), first, last);
        other.adjust_last_iterator_on_deletion(first, last);
    }

    friend bool operator==(const forward_list2& lhs, const forward_list2& rhs)
    {
        return lhs.m_list == rhs.m_list;
    }

private:
    void insert_to_empty(size_t count, const T& value)
    {
        m_last = m_list.insert_after(m_list.before_begin(), count, value);
    }

    template<class InputIt>
    void insert_to_empty(InputIt first, InputIt last)
    {
        m_last = m_list.insert_after(m_list.before_begin(), first, last);
    }

    void insert_to_empty(const std::initializer_list<T>& init)
    {
        m_last = m_list.insert_after(before_begin(), init.begin(), init.end());
    }

    void adjust_last_iterator_on_clear() noexcept
    {
        m_last = m_list.before_begin();
    }

    void adjust_last_iterator_on_insertion(const const_iterator& first, const const_iterator& last)
    {
        if (first == m_last)
            m_last = last;
    }

    void adjust_last_iterator_on_deletion(const const_iterator& first, const const_iterator& last)
    {
        if (last == cend())
            m_last = first;
    }

    void adjust_last_iterator_on_merge(const const_iterator& other_last)
    {
        if (std::next(m_last) != cend())
            m_last = other_last;
    }

    Base           m_list;
    const_iterator m_last;
};

#endif // FORWARD_LIST_2_HPP
