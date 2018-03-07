A format library with Python syntax, sane defaults, fast¹ (TBD) and reversible encoding². For C++.

------------

The biggest reason for C++17 is the accepted standard draft for character formatting, type safety and additional information about the minimum, maximum, heuristic length of arguments, thanks to `std::numeric_limits` et.al.  This information can theoretically be used at compile time when determining an optimal formatting strategy.  Additional features give us nice code without the recursive templating madness from previous standards.

The most lacking feature is floating point formatting, which internally still relies on `snprintf`. This will be dealt with as soon as [P0067R5: Elementary string conversions][P0067R5] is implemented in popular standard implementations (currently in the svn of `libstdc++`).

¹: No measurements on a full implementation yet. Current performance is `<= 1s` compared to .86s for `printf` and 1.16s for `fmtlib` on my machine.

²: That means a format string used to format some data can, if applied to basic types or supported by a correctly implemented data type, decode the same data using the same format string. A lossless specifier can be used to enforce encoding for which format->scan should not lead to imprecisions. Where this is not possible, either compilation or execution may error.

[P0067R5]: www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0067r5.html
