# blammo
Blammo strives to be a buffered/thread-safe logging system which accepts both C-style
null-terminated strings and C++ std::strings.  We will be using OpenMP for threading
because this is intended to be part of a larger project which will also be using
OpenMP.  We will also be using the agnOStic library for timing-related functions.

