# CMakeSL - The CMake scripting language
A set of C++17 libraries that implement a new scripting language for CMake.

* `doc/UserGuide.md` - The language guide
* `doc/DeveloperGuide.md` - Documentation for CMakeSL developers
* `doc/builtin_types/` - Documentation of builtin types

## Language design
* C++ish taste
* statically typed
* variables
* functions
* classes with members and methods
* set of builtin types like `string`, `list<T>`, `project`, `executable` and more..

## CMake integration
See `scripts/build_cmake.sh`

## Hello world
`CMakeLists.cmsl`:
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

## Contribution
Just clone the repo, make changes and create a pull request to the master branch.
