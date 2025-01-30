# Reflector
![license MIT](https://img.shields.io/badge/license-MIT-lightgrey.svg)
![version 1.0](https://img.shields.io/badge/version-1.0-green.svg)

A simple struct reflection framework for C++17.

## Motivation
Several programming languages provide tools and features for a type to be introspected
within the code itself, which might be useful in many diverse ways. Unfortunately,
C++17 provides very limited interfaces to explore or iterate over the properties
of a struct. This library comes in handy to widen the possibilities up when introspecting
a type in compile-time.

Please, keep in mind that the "features" exploited by this library to automatically
extract the types of a struct may not be well-defined and vary a lot between different
compilers versions. Therefore, it is prefereable to use the manually-provided reflection
interface.

## Features

- **Header-only**: just include a header and you are ready to go.
- **Automatic field registration**: no extra boilerplate code for POD structs.
- **Compile-time reflection**: all field values and offsets are mapped at compile time.

## Install
Reflector is a single-header library that can be included directly in your source
tree and depends on the [supertuple](https://github.com/rodriados/supertuple) library
for creating memory-aligned tuples with raw C++ structs.

## Usage
To use the project, you can copy source files into your own project or install it
on your system via CMake and then reference it in your code:
```cpp
#include <reflector.h>
```

## Example
If automatic reflection is possible with your configuration, it is as easy as:
```cpp
#include <reflector.h>

struct point_t { int x, y; };

const auto point = point_t {3, 4};
const auto reflection = reflector::reflection_t(point);
const auto x = reflection.get<0>(); // x == 3
```
