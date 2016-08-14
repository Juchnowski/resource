# Universal resource handling class template for C++ 
This is the unviersal class resource class. All init, cleanup routines can be expressed as 
policies, with good defaults already provided. Policies can also be used to choose storage
and copy behaviour.

## replacing `unique_ptr`
```cpp
using Res = resource<int>;
Res ptr1{new int{42}};
Res ptr2; // ptr2{nullptr}

// ptr2 = ptr1; // error
ptr2 = std::move(ptr1); // ok

// ptr2 gets deleted at the end of scope
```

### with an array
```cpp
using Res = resource<int[]>;
Res ptr1{new int[42]};
Res ptr2; // ptr2{nullptr}

// ptr2 = ptr1; // error
ptr2 = std::move(ptr1); // ok

// ptr2 gets delete[]d at the end of scope
```

### with custom deleter
```cpp
using deleter = cleanup::function_deleter<void(void*), &free>;
using Res = resource<void, deleter::impl>;
Res ptr1{malloc(1024)};
Res ptr2; // ptr2{nullptr}

// ptr2 = ptr1; // error
ptr2 = std::move(ptr1); // ok

// ptr2 gets free()d at the end of scope
```

## With your file descriptor RAII class
`unique_ptr` cannot do this - at least not without pointless allocation. 
```cpp
using deleter = cleanup::function_deleter<int(int), &close>;

using Res = resource<
	traits::handle_is_type<int, traits::nullable::yes, int, -1>,
	deleter::impl
>;

Res r1{open("/dev/random", O_RDONLY)};
Res r2; // initialized with -1

assert(*r2 == -1)
// r2 = r1; // error, but the copy policy can be convinced to use dup(2)
r2 = std::move(r1);

// r2 gets close(2)d.
```

# Why? My `unique_ptr` and `lock_guard` are better/good enough?
Most likely you're right. This was is a programming exercise.

# License
If you really want to use this, look [here](LICENSE.md).


