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

- **Header-only**: Include a single header and you are ready to go.
- **Automatic Field Registration**: No extra boilerplate code for POD structs.
- **Template Metaprogramming**: Leverages compile-time evaluation for efficiency.
- **Compile-time Reflection**: All field values and offsets are mapped at compile time.

## Install
**Reflector** is a single-header library. To use it:
- Either **copy the header** by downloading `reflector.h` from the
[latest release](https://github.com/rodriados/reflector/releases/latest) and including
it directly in your project.
- Or use **CMake integration** to install via CMake for project or system-wide use.
**Reflector** depends on the [SuperTuple](https://github.com/rodriados/supertuple)
library for creating memory-aligned tuples with raw C++ structs.

## Usage
**Reflector** provides a generic reflector type, `reflector::reflection_t`, to reflect
over any POD type. Below is an example of usage.

## Example
If automatic reflection is possible with your configuration, it is as easy as:
```cpp
#include <reflector/api.h>

struct point_t { int x, y; };

const auto point = point_t {3, 4};
const auto r = reflector::reflection_t(point);
const auto x = r.get<0>(); // x == 3
```

## License

**Reflector** is licensed under the MIT License. See
[LICENSE](https://github.com/rodriados/reflector/blob/main/LICENSE) for details.

## Contributing

Contributions are welcome! Please submit issues or pull requests via the
[GitHub repository](https://github.com/rodriados/reflector). Ensure code follows
C++17 standards and includes appropriate tests.
