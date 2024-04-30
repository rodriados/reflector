# SuperTuple
![license MIT](https://img.shields.io/badge/license-MIT-lightgrey.svg)
![version dev](https://img.shields.io/badge/version-dev-red.svg)

A simple struct reflection framework for C++17.

## Motivation
Several programming languages provide tools and features for a type to be introspected
within the code itself, which might be useful in many diverse ways. Unfortunately,
C++17 provides very limited interfaces to explore or iterate over the properties
of a struct. This library comes in handy to widen the possibilities up when introspecting
a type in compile-time.

Please, keep in mind that the "features" exploited by this library to automatically
extract the types of a struct may not be well-defined and vary a lot between different
compilers versions. Therefore, it is prefereable to use the manually-descripted reflection
interface.

## Install
This library depends on the [supertuple](https://github.com/rodriados/supertuple)
library for creating tuples memory-aligned with raw C++ structs. Besides that, the
only other dependency is a C++17-compiler to be installed on your system. As a header-only
library, you may as well directly download or copy the files into your own project
or clone it following the steps below:
```bash
git clone --recursive-submodules https://github.com/rodriados/reflector
```

## Usage
To use the project, you can copy source files into your own project or install it
on your system and then reference it in your code:
```cpp
#include <reflector.hpp>
```
