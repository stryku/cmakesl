# CMakeSL - The CMake scripting language
[![Build Status](https://travis-ci.org/stryku/cmakesl.svg?branch=master)](https://travis-ci.org/stryku/cmakesl)

A set of C++17 libraries that implement a new scripting language for CMake.


# Beware
CMakeSL is not supported by the CMake project. It's not an official part of the CMake.


# Motivation and goals
To make CMake environment intuitive and developer-friendly.

The CMakeSL's purpose is to introduce a new scripting language and the scripting language only.
All well known CMake's concepts like targets, exporting, importing etc. are going to be preserved.

Ideally, using CMakeSL should not require to learn a new language or install anything.
Users with C/C++ (or similar) background can start scripting right away.
C++ish style of the language allows to use tools like clang-format, doxygen and any other
(that does not require semantic information) out-of-the-box.

For tasks that require semantic info, CMakeSL provides ready-made tools (a library with C interface): indexer and syntax completer.


# Useful links
* [The language guide](https://github.com/stryku/cmakesl/blob/master/doc/UserGuide.md)
* [Documentation of builtin types](https://github.com/stryku/cmakesl/tree/master/doc/builtin_types)
* [Examples of tools usage and a simple CMakeSL-based project](https://github.com/stryku/cmakesl/tree/master/examples)
* [Documentation for CMakeSL developers](https://github.com/stryku/cmakesl/blob/master/doc/DeveloperGuide.md)

# Language design
* Imperative - keep your mind in one paradigm while switching between C/C++ and CMake code.
* C++ish taste - The syntax and semantic are very similar to the ones from C++, so a developer who has any experience with this language will fairly quickly pick the CMakeSL up. You have variables, classes, scopes, namespaces (soon), modules (soon) and a lot of other features known from C++. Another advantage of such approach is that CMakeSL is compatible with a lot of C++ tools like syntax highlighting, clang-format, doxygen etc. See [The language guide](https://github.com/stryku/cmakesl/blob/master/doc/UserGuide.md) for more.
* Statically typed - be notified about the mistakes before the execution even starts.
* Set of builtin types like `string`, `list<T>`, `project`, `executable` and more. Combined with static typing, makes CMake's interface powerful and easy to use at the same time.


## Hello world
A top-level `CMakeLists.cmsl`:
```cpp
int main()
{
    cmake_minimum_required(version(3, 14, 3));

    project hello_world = project("Hello world");

    list<string> sources = { "main.cpp" };
    hello_world.add_executable("hw_exec", sources);

    return 0;
}
```


# Tools
CMakeSL is written with tooling in mind. While you can use doxygen, clang-format and possibly other tools out-of-the-box,
tools that require language semantic (e.g. clang's code completion) will not work (because e.g. CMakeSL's fundamental types are in fact classes, with methods).
That's why, CMakeSL provides its own tools for indexing and code completion.
Indexer and completer are parts of [cmsl_tools](https://github.com/stryku/cmakesl/tree/master/tools/lib) library. The library has C language interface, so it can be used pretty much everywhere.

Check out [examples/](https://github.com/stryku/cmakesl/tree/master/examples) to see an example usage of the indexer and syntax completion.


# Building and CMake integration
Building CMakeSL libraries is simple as:
```sh
git clone https://github.com/stryku/cmakesl
cd cmakesl
mkdir build && cd build
cmake .. [-DCMAKESL_WITH_TESTS=ON/OFF] [-DCMAKESL_WITH_TOOLS=ON/OFF] [-DCMAKESL_WITH_EXAMPLES=ON/OFF]
make
```
* `CMAKESL_WITH_TESTS=ON` enables building tests
* `CMAKESL_WITH_TOOLS=ON` enables building tools library
* `CMAKESL_WITH_EXAMPLES=ON` enables building example usage of indexer and syntax completion tools

This will build only the libraries. To integrate CMakeSL into the CMake, more work has to be done:
```sh
# Clone CMake codebase
git clone https://gitlab.kitware.com/cmake/cmake.git
cd cmake
# CMakeSL bases on this particular release
git checkout v3.14.3

# Clone CMakeSL in the `Source` directory
cd Source
git clone https://github.com/stryku/cmakesl
cd cmakesl
# Checkout to a specific version if need.
git checkout v0.0

# Apply changes to CMake code
cd ../..
git apply Source/cmakesl/cmake_integration/cmake_changes.patch

# Build CMake. C++17 compiler is required
cd ..
mkdir build install
cd build
cmake ../cmake -DCMAKE_CXX_COMPILER=<compiler supporting C++17> -DCMAKE_INSTALL_PREFIX=../install
make install -j
cd ..
# Now, in install/bin you have the `cmake` binary that supports CMakeSL
```

An alternative way to build CMake is to use a ready-made script from the [scripts/](https://github.com/stryku/cmakesl/tree/master/scripts) directory:
```sh
wget https://raw.githubusercontent.com/stryku/cmakesl/master/scripts/build_cmake.sh
chmod +x build_cmake.sh
./build_cmake.sh clang++-7
```


## Dependencies
The only dependency that CMakeSL needs is C++17 (and python3, but its dependency probably is going to be removed). No external libraries are used.


## Contribution
Just clone the repo, make changes and create a pull request to the master branch.
