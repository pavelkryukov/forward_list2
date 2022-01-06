[![codecov](https://codecov.io/gh/pavelkryukov/forward_list2/branch/main/graph/badge.svg?token=5KtoC3WASR)](https://codecov.io/gh/pavelkryukov/forward_list2)

# forward_list2

Wrapper around C++ [`std::forward_list`](https://en.cppreference.com/w/cpp/container/forward_list) which adds an iterator to the last element.

The idea has been proposed and discussed here: [cpp-ru/ideas#487](https://github.com/cpp-ru/ideas/issues/487).


### Extra functions
```c++
const_reference back() const;

const_iterator before_end() const noexcept;
const_iterator cbefore_end() const noexcept;

void push_back(const T& value);
void push_back(T&& value);

template<class... Args>
reference emplace_back(Args&&... args);
```

### Price
* Compute time overheads to maintain the iterator the last element.
* Extra O(N) traversals on copy assignment and sorting.
* Memory size overhead on empty container:
```c++
static_assert(sizeof(std::forward_list<int>) == sizeof(void*));
static_assert(sizeof(forward_list2<int>) == 2 * sizeof(void*));
```

### Limitations

* No `reference back();` is available since it would require passing non-constant iterator to `erase_after`.

### Impact

* Identified and fixed [PR libstdc++/103853](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=103853).
