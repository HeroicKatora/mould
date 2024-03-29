A format library with Python syntax, sane defaults, fast. For compile-time C++.

Compatible with C++17 and upwards.

-----------

Implements the `formatlib` benchmark in [speed.cpp](./test/run.cpp).
Tested with `-std=c++20` as it includes bit tricks for faster code.

| test name | run time | executable size (stripped) |
|-----------|---------:|---------------------------:|
| gcc       | 0.253s   | 135kB |
| clang     | 0.140s   | 43kB |

------------

C++17 accepted `fmt` in its worst form, with runtime format templates. The
independent library's implementation later at least added type-checked
arguments but the internals are not constexpr ready enough to where the
compiler could really optimize on.

We go further. The biggest difference to the accepted standard draft is gather
all character formatting, type safety and additional information about the
minimum, maximum, heuristic length of arguments, `std::numeric_limits` et.al.
This information can theoretically be used at compile time when determining an
optimal formatting strategy. The library allows types to provide a formatter
lookup, that finds any function based on the statically encoded format
arguments.
