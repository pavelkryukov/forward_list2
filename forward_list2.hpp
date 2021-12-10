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
        m_list(), m_last(m_list.before_begin())
    {

    }

    explicit forward_list2(const Allocator& alloc) :
        m_list(alloc), m_last(m_list.before_begin())
    {

    }

    forward_list2(size_type count, const T& value, const Allocator& alloc = Allocator()) :
        forward_list2(alloc)
    {
        m_last = m_list.insert_after(m_last, count, value);
    }

    template<class InputIt>
    forward_list2(InputIt first, InputIt last, const Allocator& alloc = Allocator()) :
        forward_list2(alloc)
    {
        m_last = m_list.insert_after(m_last, first, last);
    }

    forward_list2(const forward_list2& other) : 
        forward_list2(std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.m_list.get_allocator()))
    {
        m_last = m_list.insert_after(m_last, other.begin(), other.end());
    }

    forward_list2(const forward_list2& other, const Allocator& alloc) : 
        forward_list2(alloc)
    {
        m_last = m_list.insert_after(m_last, other.begin(), other.end());
    }

    forward_list2(forward_list2&& other) :
        m_list(std::move(other.m_list)), m_last(std::move(other.m_last))
    {
        other.clear();
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
        m_last = m_list.insert_after(before_begin(), init.begin(), init.end());
    }

    void clear() noexcept
    {
        m_list.clear();
        m_last = m_list.before_begin();
    }

    iterator insert_after(const_iterator pos, const T& value )
    {
        auto inserted_pointer = m_list.insert_after(pos, value);
        if (pos == m_last)
            m_last = inserted_pointer;

        return inserted_pointer;
    }

    iterator insert_after(const_iterator pos, size_type count, const T& value)
    {
        auto inserted_pointer = m_list.insert_after(pos, count, value);
        if (pos == m_last)
            m_last = inserted_pointer;

        return inserted_pointer;
    }

    template<class InputIt>
    iterator insert_after(const_iterator pos, InputIt first, InputIt last)
    {
        auto inserted_pointer = m_list.insert_after(pos, first, last);
        if (pos == m_last)
            m_last = inserted_pointer;

        return inserted_pointer;
    }

    reference front()             { return *begin(); }
    const_reference front() const { return *begin(); }

    reference back()             { return *before_end(); }
    const_reference back() const { return *before_end(); }

    iterator before_begin()              noexcept { return m_list.before_begin(); }
    const_iterator before_begin()  const noexcept { return m_list.before_begin(); }
    const_iterator cbefore_begin() const noexcept { return m_list.cbefore_begin(); }

    iterator begin()              noexcept { return m_list.begin(); }
    const_iterator begin()  const noexcept { return m_list.begin(); }
    const_iterator cbegin() const noexcept { return m_list.cbegin(); }

    iterator before_end()              noexcept { return m_last; }
    const_iterator before_end()  const noexcept { return m_last; }
    const_iterator cbefore_end() const noexcept { return m_last; }

    iterator end()              noexcept { return m_list.end(); }
    const_iterator end()  const noexcept { return m_list.end(); }
    const_iterator cend() const noexcept { return m_list.cend(); }

    bool empty() const { return m_list.empty(); }

private:
    Base     m_list;
    iterator m_last;
};
