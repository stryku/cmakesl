# CMakeSL - The CMake scripting language
[![Build Status](https://travis-ci.org/stryku/cmakesl.svg?branch=master)](https://travis-ci.org/stryku/cmakesl)

A set of C++17 libraries that implement a new scripting language for CMake.

Table of Contents
=================

   * [CMakeSL - The CMake scripting language](#cmakesl---the-cmake-scripting-language)
   * [Beware](#beware)
   * [Motivation and goals](#motivation-and-goals)
   * [Useful links](#useful-links)
   * [Language design](#language-design)
   * [CMakeSL Hello World](#cmakesl-hello-world)
   * [Useful Hello World](#useful-hello-world)
   * [Tools](#tools)
      * [Indexer](#indexer)
      * [Syntax completion](#syntax-completion)
   * [Building and CMake integration](#building-and-cmake-integration)
   * [Dependencies](#dependencies)
   * [Contribution](#contribution)


# Beware
CMakeSL is not supported by the CMake project. It's not an official part of the CMake.


# Motivation and goals
To possibly make CMake environment intuitive and developer-friendly.

The CMakeSL's purpose is to introduce a new scripting language and the scripting language only.
All well known CMake's concepts like targets, exporting, importing etc. are going to be preserved.

Ideally, using CMakeSL should not require to learn a new language or install anything.
Users with C/C++ (or similar) background can start scripting right away.
C++ish style of the language allows to use tools like clang-format, doxygen and any other
(that does not require semantic information) out-of-the-box.

For tasks that require semantic info, CMakeSL provides ready-made tools (a library with C interface): indexer and syntax completer.


# Useful links
* [The language guide](https://github.com/stryku/cmakesl/blob/master/doc/UserGuide.md)
* [Documentation of builtin stuff](https://github.com/stryku/cmakesl/tree/master/doc/builtin)
* [Examples of tools usage and a simple CMakeSL-based project](https://github.com/stryku/cmakesl/tree/master/examples)
* [Documentation for CMakeSL developers](https://github.com/stryku/cmakesl/blob/master/doc/DeveloperGuide.md)

# Language design
* Imperative - keep your mind in one paradigm while switching between C/C++ and CMake code.
* C++ish taste - The syntax and semantic are very similar to the ones from C++, so a developer who has any experience with this language will fairly quickly pick the CMakeSL up. You have variables, classes, scopes, namespaces (soon), modules (soon) and a lot of other features known from C++. Another advantage of such approach is that CMakeSL is compatible with a lot of C++ tools like syntax highlighting, clang-format, doxygen etc. See [The language guide](https://github.com/stryku/cmakesl/blob/master/doc/UserGuide.md) for more.
* Statically typed - be notified about the mistakes before the execution even starts.
* Set of builtin types like `string`, `list<T>`, `project`, `executable` and more. Combined with static typing, makes CMake's interface powerful and easy to use at the same time.


# CMakeSL Hello World
```cpp
int main()
{
    cmake::message("Hello CMakeSL World!");
    return 0;
}
```

# Useful Hello World
A top-level `CMakeLists.cmsl`:
```cpp
int main()
{
    cmake::minimum_required(cmake::version(3, 14, 3));

    auto hello_world = cmake::project("Hello world");

    list<string> sources = { "main.cpp" };
    hello_world.add_executable("hw_exec", sources);

    return 0;
}
```


# Tools
CMakeSL is written with tooling in mind. While you can use doxygen, clang-format and possibly other tools out-of-the-box,
other tools that require language semantic (e.g. clang's code completion) will not work (because e.g. CMakeSL's fundamental types are in fact classes, with methods).
That's why, CMakeSL provides its own tools for indexing and code completion. These tools provide C language interface, so they can be used pretty much everywhere

Indexer and completer are parts of [cmsl_tools](https://github.com/stryku/cmakesl/tree/master/tools/lib) library.

Check out [examples/](https://github.com/stryku/cmakesl/tree/master/examples) to see an example usage of the indexer and syntax completion.

## Indexer
Indexes the source. Useful for implementing e.g. 'jump to definition' util in an IDE. For given source:
```cpp
class hello_world
{
    int foo_getter()
    {
        return foo;
    }

    int foo;
};

int foo() { return 0; }

int bar;

int main(int param_1, list<string> param_2)
{
    hello_world hw;
    hw.foo = param_1;
    return hw.foo_getter();
}
```

produces such entries:
```
entry type: type, source begin: 78, source end: 81, destination position: 91, destination path: ~/doc/builtin/int.cmsl
    int foo;
    ^^^
entry type: type, source begin: 24, source end: 27, destination position: 91, destination path: ~/doc/builtin/int.cmsl
    int foo_getter()
    ^^^
entry type: identifier, source begin: 62, source end: 65, destination position: 82, destination path: <unknown source>
        return foo;
               ^^^
entry type: type, source begin: 91, source end: 94, destination position: 91, destination path: ~/doc/builtin/int.cmsl
int foo() { return 0; }
^^^
entry type: type, source begin: 116, source end: 119, destination position: 91, destination path: ~/doc/builtin/int.cmsl
int bar;
^^^
entry type: type, source begin: 126, source end: 129, destination position: 91, destination path: ~/doc/builtin/int.cmsl
int main(int param_1, list<string> param_2)
^^^
entry type: type, source begin: 135, source end: 138, destination position: 91, destination path: ~/doc/builtin/int.cmsl
int main(int param_1, list<string> param_2)
         ^^^
entry type: type, source begin: 148, source end: 152, destination position: 83, destination path: ~/doc/builtin/list.cmsl
int main(int param_1, list<string> param_2)
                      ^^^^
entry type: type, source begin: 153, source end: 159, destination position: 46, destination path: ~/doc/builtin/string.cmsl
int main(int param_1, list<string> param_2)
                           ^^^^^^
entry type: type, source begin: 176, source end: 187, destination position: 6, destination path: <unknown source>
    hello_world hw;
    ^^^^^^^^^^^
entry type: identifier, source begin: 196, source end: 198, destination position: 188, destination path: <unknown source>
    hw.foo = param_1;
    ^^
entry type: class member identifier, source begin: 199, source end: 202, destination position: 82, destination path: <unknown source>
    hw.foo = param_1;
       ^^^
entry type: identifier, source begin: 205, source end: 212, destination position: 139, destination path: <unknown source>
    hw.foo = param_1;
             ^^^^^^^
entry type: operator function, source begin: 203, source end: 204, destination position: 3031, destination path: ~/doc/builtin/int.cmsl
    hw.foo = param_1;
           ^
entry type: identifier, source begin: 225, source end: 227, destination position: 188, destination path: <unknown source>
    return hw.foo_getter();
           ^^
entry type: function call name, source begin: 228, source end: 238, destination position: 28, destination path: <unknown source>
    return hw.foo_getter();
              ^^^^^^^^^^
```

As you can see, entries carry all information that are needed to jump to the entry declaration: begin and end of the entry in the source, destination position and file of where you'd need to jump. For builtin stuff, it gives the path to the builtin type documentation file, so implementing in-editor documentation support is very straightforward.

See [the index.c example implementation](examples/index.c) for details.

## Syntax completion
Currently, completion works only when given source has no errors.

For given source:
```cpp
class hello_complete_world
{};

int foo()
{}

int bar;

int main(int param_1, list<string> param_2)
{
    // Complete here
}
```
and completion triggered inside the `main` function, gives such entries:
```
if
for
while
list<string>
bool
hello_complete_world
double
int
string
void
bar
param_2
foo
main
param_1
```
which are basically what you would want to write in a context of standalone statement.

See [the complete.c example implementation](examples/complete.c) for details.


# Building and CMake integration
Building CMakeSL libraries is simple as:
```sh
git clone https://github.com/stryku/cmakesl
cd cmakesl
mkdir build && cd build
cmake .. [-DCMAKESL_WITH_TESTS=ON/OFF] [-DCMAKESL_WITH_TOOLS=ON/OFF] [-DCMAKESL_WITH_EXAMPLES=ON/OFF] [-DCMAKESL_WITH_DOCS=ON/OFF]
make
```
* `CMAKESL_WITH_TESTS=ON` enables building tests.
* `CMAKESL_WITH_TOOLS=ON` enables building and installing tools library.
* `CMAKESL_WITH_EXAMPLES=ON` enables building example usage of indexer and syntax completion tools.
* `DCMAKESL_WITH_DOCS=ON` enables building and installing documentation.

This will build only the libraries. In order to integrate CMakeSL into the CMake codebase, more work has to be done:
```sh
# Clone CMake codebase.
git clone https://github.com/stryku/cmake_for_cmakesl cmake
cd cmake
# CMakeSL bases on this particular branch.
git checkout cmakesl

# Clone CMakeSL in the `Source` directory.
cd Source
git clone https://github.com/stryku/cmakesl
cd cmakesl
# Checkout to a specific version if need. If not checked out, trunk will be built.
# git checkout v0.0

# Build CMake. C++17 compiler is required.
cd ..
mkdir build install
cd build
cmake ../cmake -DCMAKE_CXX_COMPILER=<compiler supporting C++17> -DCMAKE_INSTALL_PREFIX=../install
make install -j
cd ..
# Now, in install/bin you have the `cmake` binary that supports CMakeSL.
```

An alternative way to build CMake is to use a ready-made script from the [scripts/](https://github.com/stryku/cmakesl/tree/master/scripts) directory:
```sh
wget https://raw.githubusercontent.com/stryku/cmakesl/master/scripts/build_cmake.sh
chmod +x build_cmake.sh
./build_cmake.sh clang++-7
```

With the `cmake` binary that supports CMakeSL execution, the [cmsl_example](https://github.com/stryku/cmakesl/tree/master/examples/cmsl_example) can be built.
```sh
cd install/bin
cp -r ../../cmake/Source/cmakesl/examples/cmsl_example .
mkdir build && cd build
./../cmake ../cmsl_example
make -j
./hw_exec
```


# Dependencies
The only dependencies that CMakeSL needs is C++17 and python3. No external C++ libraries are used.


# Contribution
Just clone the repo, make changes and create a pull request to the master branch.
