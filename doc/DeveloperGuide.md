# CMakeSL Developer Guide

## C++
C++17 is used. No particular compilator is required.

## Libraries
A set of libraries are created in the project:
* common - common stuff like algorithms etc.
* errors - this one was introduced to be able to shadow mock the `errors_observer` class. Now, it's considered to remove this library, or at least, remove shadow mocking and just create an interface for `errors_observer` (that's still a TODO).
* lexer - this one takes a raw `std::string` as CMakeSL script source code and returns a vector of tokens.
* ast - this one takes the vector of tokens and returns an ast tree.
* sema - this one takes the ast tree and returns sema tree.
* exec - finally, this one takes the sema tree and executes it.
* cmsl_tools - a library with a C interface that provides functions for syntax completion and source indexing.

Additionally, executables are created:
* cmakesl - It's a simple exec that takes path to sources and executes CMakeSL script outside the CMake world. It's used to quickly execute some scripts and see what happens.

## Testing
To enable tests, set option `CMAKESL_WITH_TESTS=ON`.

While debugging, `sema::dumper` class can be useful. It prints a built semantic tree to the given output stream. It can help analyzing weird behaviour.

I the `global_executor::execute` (or other method that deals with semantic tree) after compiling the source, add:
```cpp
{
    sema::dumper d{ std::cout };
    compiled->sema_tree().visit(d);
}
```

## Contribution
Just grab sources, make changes and create a pull request to `master` branch.
