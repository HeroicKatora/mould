A format library with Python syntax, sane defaults, fast¹ (TBD) and reversible encoding². For C++.

¹:
²: That means a format string used to format some data can, if applied to basic types or supported by a correctly implemented data type, decode the same data using the same format string. A lossless specifier can be used to enforce encoding for which format->scan should not lead to imprecisions. Where this is not possible, either compilation or execution may error.

