# expected

Single header std::expected implementation using C++20.

The library implements these 2 proposals:

-   [P0323](https://wg21.link/p0323) defines the basic interface for std::expected.
-   [P2505](https://wg21.link/p2505) defines the monadic inteface extensions for std::expected.

This implementation provides expected in namespace rd.
So, for any use of std::expected, use rd::expected.

## What std::expected does

std::expected is a mechanism for error handling. This is very similar to rust's std::result.
std::expected basically contains one of Result or Error.

## Why not just use exceptions

There are probably 2 reasons for not "always" using exceptions:

-   Exceptions are forbidden in many codebases
-   Exceptions are too costly when any exception case arise.

While writing any library, this poses a big problem for error handling. In
library, if we choose to throw exception on error condition, we are making the
choice for library user. Error condition in library can or can not be an
exceptional situation in context of the application that uses that library.

std::expected provides a standard alternative for exception that can be used
for error condition that are not exceptional in context of application.

While saying this, this should be noted that exceptions are great for panic
conditions. And there exists many obvious panic conditions, for which only
exceptions should be used.

## What about other alternatives

error_code used to be simplest error handling mechanism. Historically,
error_code is a simple integer, that is not good for rich error context.

Nowadays, C++ header &lt;system_error> provides richer error code. However,
error_code leads to monopolization of return channel. And thus value need to be
outputted with some other channel.

It also forces you to write if condition after each function call.

## Never empty guarantee

std::expected guarantees that it is never going to be empty. i.e., it would
either contain one of value or error at any point of time.
This makes it really easy to use.

## Simple code with rd::expected

```cpp
auto get_file_handle(std::string_view path) -> rd::expected<file, fail_reason>;

auto read_file(file f) -> rd::expected<std::string, fail_reason>;

auto to_int(std::string str) -> rd::expected<int, fail_reason>;

auto increment(int x) -> int;

auto read_int_with_increment(std::string_view path) -> rd::expected<int, fail_reason>{
  return get_file_handle(path)
         .and_then(read_file)
	 .and_then(to_int)
	 .transform(increment);
}
```

## Documentation

### rd::expected

```cpp
template <typename T, typename E>
class expected
```

T, E can't be a reference type, as they are not supported in paper too.

#### Member types

```cpp
using value_type = T;
using error_type = E;
using unexpected_type = unexpected<E>;

template <class U>
using rebind = expected<U, error_type>;
```

#### Constructors

Default constructor:

```cpp
constexpr expected();
```

Copy constructor:

```cpp
constexpr expected(expected const&);
```

Move constructor:

```cpp
constexpr expected(expected &&);
```

Expected constructor:

U should be convertible to T, and G should be convertible to E.

```cpp
template <class U, class G>
constexpr expected(expected<U, G> const&);

template <class U, class G>
constexpr expected(expected<U, G> &&);
```

Value constructor:

Constructor for expected \\&lt;T , E> that accepts value to initialize T(value
part of expected).

```cpp
template <class U = T>
constexpr expected(U&& v);
```

Unexpected constructor:

Constructor for expected&lt;T, E> that accepts rd::unexpected instance to
initialize E(error part of expected).

```cpp
template <class G>
constexpr expected(unexpected<G> const&);

template <class G>
constexpr expected(unexpected<G> &&);
```

in_place_t constructor:

Constructor for expected&lt;T, E> that accepts in_place tag and arguments(to
be forwarded) to initialize T.

```cpp
template <class... Args>
constexpr expected(std::in_place_t, Args&&... args);

template <class U, class... Args>
constexpr expected(std::in_place_t, std::initializer_list<U>, Args&&... args);
```

unexpect_t constructor:

Constructor for expected&lt;T, E> that accepts rd::unexpect tag and and arguments(to
be forwarded) to initialize E.

```cpp
template <class... Args>
constexpr expected(rd::unexpect_t, Args&&... args);

template <class G, class... Args>
constexpr expected(rd::unexpect_t, std::initializer_list<G>, Args&&... args);
```

#### Assignment

copy assignment:

```cpp
constexpr expected& operator=(expected const&);
```

move assignment:

```cpp
constexpr expected& operator=(expected &&);
```

Value assignment:

Assigns value to expected

```cpp
template <class U = T>
constexpr expected& operator=(U&& rhs);
```

rd::unexpected assignment:

Assigns error to expected

```cpp
template <class G>
constexpr expected& operator=(unexpected<G> const&);

template <class G>
constexpr expected& operator=(unexpected<G> &&);
```

#### Modifiers

emplace:

Accepts args to constructor new value for expected and assign to it.

```cpp
template <class... Args>
constexpr T& emplace(Args&&... args);

template <class U, class... Args>
constexpr T& emplace(std::initializer_list<U>, Args&&...);
```

#### Swap

swap member function and friend function

#### Observers

```cpp
// For accessing T's members

// precondition: has_value() == true
constexpr T const* operator->() const noexcept;

// precondition: has_value() == true
constexpr T* operator->() noexcept;
```

```cpp
// Getting reference to T

// precondition: has_value() == true
constexpr T const& operator*() const& noexcept;

// precondition: has_value() == true
constexpr T& operator*() & noexcept;

// precondition: has_value() == true
constexpr T&& operator*() && noexcept;

// precondition: has_value() == true
constexpr T const&& operator*() const&& noexcept;
```

```cpp
// Query if value exists

constexpr explicit operator bool() const noexcept;

constexpr bool has_value() const noexcept;
```

```cpp
// Get value, if not exists throws exception rd::bad_expected_access(error())

constexpr T const& value() const&;
constexpr T& value() &;
constexpr T&& value() &&;
constexpr T const&& value() const&&;
```

```cpp
// Get error, (undefined behavior if error not exists)

constexpr E const& error() const&;
constexpr E& error() &;
constexpr E&& error() &&;
constexpr E const&& error() const&&;
```

```cpp
// Get the value, if value doesn't exist return v

template <class U>
constexpr T value_or(U&& v) const&;

template <class U>
constexpr T value_or(U&& v) &&;
```

#### Monadic

and_then:

F should be invocable with value type of expected and should return expected
whose error type should be E.
It returns error of current expected, if error is there, other returns the
result of invoking f with value.

```cpp
template <class F>
constexpr auto and_then(F&& f) &;

template <class F>
constexpr auto and_then(F&& f) &&;

template <class F>
constexpr auto and_then(F&& f) const &;

template <class F>
constexpr auto and_then(F&& f) const &&;
```

or_else:

F should be invocable with error type of expected and should return expected
whose value type should be T.

If `invoke_result_t<F, E>` is any specialization of expected (whose value type should be same as T), then
if a value is there, value is returned wrapped in invoke_result_t of F otherwise returns the result of
invoking f.

If `invoke_result_t<F, E>` is void, then, if error is there, then F is invoked with error.
Current expected is returned as result.

```cpp
template <class F>
constexpr auto or_else(F&& f) &;

template <class F>
constexpr auto or_else(F&& f) &&;

template <class F>
constexpr auto or_else(F&& f) const &;

template <class F>
constexpr auto or_else(F&& f) const &&;
```

transform:

F should be invocable with value type of current expected. If current expected
contains error the resultant expected contains that error otherwise contains the
result of invocation of current expected value with F.

```cpp
template <class F>
constexpr auto transform(F&& f) &;

template <class F>
constexpr auto transform(F&& f) &&;

template <class F>
constexpr auto transform(F&& f) const &;

template <class F>
constexpr auto transform(F&& f) const &&;
```

transform_error:

F should be invocable with error type of current expected. If current expected
contains value the resultant expected contains that value otherwise contains the
result of invocation of current expected error with F.

```cpp
template <class F>
constexpr auto transform_error(F&& f) &;

template <class F>
constexpr auto transform_error(F&& f) &&;

template <class F>
constexpr auto transform_error(F&& f) const &;

template <class F>
constexpr auto transform_error(F&& f) const &&;
```

#### Equality

```cpp
template <class U, class G>
constexpr friend bool operator==(expected const&, expected<U, G> const&);

template <class U>
constexpr friend bool operator==(expected const&, U const&);

template <class G>
constexpr friend bool operator==(expected const&, rd::unexpected<G> const&);
```

### rd::unexpected

```cpp
template <typename T>
class unexpected
```

#### Member types

```cpp
using value_type = T
```

#### Constructors

Copy constructor:

```cpp
constexpr unexpected(unexpected const&);
```

Move constructor:

```cpp
constexpr unexpected(unexpected&&);
```

in_place_t constructor:

It accepts a std::in_place tag and constructs T with args.

```cpp
template <typename ...Args>
constexpr unexpected(std::in_place_t, Args&&... args);

template <typename U, typename ...Args>
constexpr unexpected(std::in_place_t, std::initializer_list<U>, Args&&... args);
```

value constructor:

Constructs T with err.

```cpp
template <typename Err>
constexpr unexpected(Err&& err);
```

#### Observer

```cpp
// value(): gets the stored T value

constexpr T & value() &;
constexpr T const& value() const&;
constexpr T && value() &&;
constexpr T const&& value() const&&;
```

#### Equality operator

```cpp
template <class E2>
friend constexpr bool operator==(unexpected const& x, unexpected<E2> const& y);
```

### rd::bad_expected_access

bad_expected_access derives from exception. Stores the error value.

```cpp
template<class E>
class bad_expected_access;
```

```cpp
E& error() & noexcept;
E const& error() const& noexcept;
E&& error() && noexcept;
E const&& error() const&& noexcept;
```

### rd::unexpect_t

This is just a tag type, to signify constructing error for expected.

Library also exposes a unexpect variable of type rd::unexpect_t.

```cpp
inline constexpr unexpect_t unexpect{};
```
