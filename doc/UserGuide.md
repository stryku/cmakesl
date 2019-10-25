# CMakeSL User guide

## CMakeLists.txt vs CMakeLists.cmsl
CMake binary expects that CMakeSL scripts are named `CMakeLists.cmsl`. Based on extension (`cmsl` not `txt`) it knows that the scripts are written in CMakeSL, not the 'old' CMake language.

## Entry point of scripts execution
Every `CMakeLists.cmsl` script has to have a main function. It's the entry point of script execution.

The main function of the top-level script has to have signature taking no parameters and returning an int:
```cpp
int main() { ... }
```

Main function of scripts in subdirectories are allowed to have any signature, but the function has to be named `main`. E.g.:
```cpp
cmake::library main(cmake::project& top_lvl_project, string some_additional_parameter)
{
    ...
}
```

Script is allowed to have one, and exactly one, main function.

# Builtin types
Here's a list of currently implemented builtin types:
* void
* bool
* int
* double
* string
* extern<T>
* list<T>
* cmake::option
* cmake::version
* cmake::project
* cmake::executable
* cmake::library

All types are classes, even the primitives (bool, int, double).

For detailed documentation of the types please refer to files under [doc/builtin](builtin).

For the list of builtin functions, please see [the cmake namespace doc file](builtin/cmake.cmsl).

## void
Well known from C/C++ (and others). Used to indicate that a function doesn't return anything.

## bool
Not so much to say. A bool type that does what you expect from bool.
It's a class, so you can e.g. call `to_string()` method:
```cpp
bool foo = true;
string bar = foo.to_string();
```

## int
A signed 64 bit int. Has `to_string`, as well:
```cpp
int foo = 42;
string bar = foo.to_string();
```

## double
A typical double. Not much to say.

## string
A string type implementing basic operations from C++' `std::string` and some from the Python's one.

## extern<T>
A type used to get variables passed to cmake command line. E.g. for a such call:
```
cmake .. -DFOO="some string"
```
You can get the `FOO` value like this:
```cpp
auto foo = extern<string>("FOO");
if(foo.has_value()) {
    cmake::message(foo.value());
}
```

If `FOO` value was not passed, `foo.has_value()` returns `false`.

## list<T>
A generic list.
```cpp
list<string> some_list;
some_list += "foo";
some_list.push_back("bar");

string qux = "qux";
list<string> other_list = { "baz", qux };

some_list += other_list;
```

List is generic, so you can have a list of lists:
```cpp
list<list<int>> matrix = {
    { 1, 2, 3 },
    { 4, 42, 6 },
    { 7, 8, 9 }
};

matrix.at(1).at(1) = 5;
```

An array subscript operator (`matrix[1][1]`) is not supported yet.

## cmake::option
An equivalent of CMake's `option()` command:
```cpp
auto with_tests = option("MY_PROJECT_WITH_TESTS", "When ON, tests will be built", false);
if(with_tests.value()) {
    add_subdirectory("test");
}
```

## cmake::version
A type representing version. You passes it to `cmake_minimum_required`:
```cpp
cmake::version ver = cmake::version(3, 14);
cmake::minimum_required(ver);
string str_version = ver.to_string();
```

## project
Registers a project in the build system. It registers it only at `project("some name")` constructor, so you can copy the variable around, without duplicating more projects.
```cpp
cmake::project foo = cmake::project("The Foo");
foo.add_executable(...);
```

## library
Represents library. `project::add_library` returns an instance of it, so other libraries and executables can link to it.
```cpp
cmake::project foo = cmake::projecr("The Foo");

cmake::library bar_lib = foo.add_library("bar_lib", { "bar.cpp" });
cmake::library baz_lib = foo.add_library("baz_lib, { "baz.cpp" });

baz_lib.link_to(bar_lib);
```

## executable
Similar to library but other executables and libraries can't link to it (it's actually ensured by type safety - `library/executable::link_to` accepts only instances of the `library` type):
```cpp
cmake::project foo = cmake::projecr("The Foo");

cmake::library bar_lib = foo.add_library("bar_lib", { "bar.cpp" });
cmake::library baz_exec = foo.add_executable("baz_exec", { "baz.cpp" });

baz_exec.link_to(bar_lib);
```

# User types
A class with members and methods can be defined by user:
```cpp
class foo
{
    int bar;

    auto multiplied_bar(int mul)
    {
        return bar * mul;
    }
};
```

# Type deduction
The `auto` is not an actual type, but it can be used in a context where the type can be deduced:
```cpp
auto foo = 42; // Deduced to int.

// Deduced to void.
auto bar()
{}

// Deduced to int.
auto baz()
{
    return foo;
}

// The deduction can be nested. Deduced to int.
auto qux(bool flag)
{
    if(flag)
    {
        return baz();
    }

    return 42;
}

// Error, the function which return type should be deduced,
// can not be called recursively.
auto top()
{
    return top();
}

// Error, the return type can not be deduced because
// the return expressions evaluate to different types.
auto kek(bool flag)
{
    if(flag)
    {
        return 42;
    }

    return "some string";
}
```

# Designated initializers
When the expected type of a variable is known, designated initializers can be used to initialize it:
```cpp
class foo
{
    int bar;
    string baz;
};

// Not all members need to be initialized.
foo qux = { .bar = 42 };

void top(foo kek)
{}

int main()
{
    top({ .baz = "some string" });
    reutrn 0;
}
```

# Import/export
Besides an obvious division of CMakeSL scripts into `CMakeLists.cmsl` in subdirectories, user can create a script that e.g. exports some functionality or variables.

An example file that provides util to create desired library name `libs_utils.cmsl`:
```cpp
// The 'export' indicates that this function should be importable.
namespace utils
{
export string create_library_name(string base_name)
{
    return "my_project_" + base_name;
}
}
```

And an example usage, the root `CMakeLists.cmsl`:
```cpp
// Import the file with libs utils.
import "libs_utils.cmsl";

int main()
{
    cmake::minimum_required(cmake::version(3, 14));
    auto project = cmake::project("My Project");

    auto lib_name = utils::create_library_name("my_lib");
    project.add_library(lib_name, { /* ... */ };

    return 0;
}
```

Currently, functions, variables and classes can be exported:
```cpp
export int foo = 42;

export int bar()
{
    return foo;
}

export class baz
{
    auto qux()
    {
        return bar();
    }
};
```

# Mixing CMakeSL with 'old' CMake
CMakeSL can be mixed with CMake at directories level. In short, in a `CMakeLists.cmsl` you can call `add_subdirectory()` with a subdirectory that contains 'old' `CMakeLists.txt`. It works (well, not exactly, it'll fully work soon) also the other way - in 'old' `CMakeLists.txt` you can call `add_subdirectory()` with a `CMakeLists.cmsl` script.

Please see [the root CMakeLists.cmsl](https://github.com/stryku/cmakesl/tree/master/CMakeLists.cmsl). There is a call `add_subdirectory("external/googletest")` which add the whole googletest library, that is later on used in the CMakeSL tests.


# Examples

## Hello world
A simple CMakeSL script:
```cpp
int main()
{
    cmake::message("Hello World!");
    return 0;
}
```

Here's one that actually does something useful:
```cpp
int main()
{
    cmake::minimum_required(version(3,14,3))

    cmake::project hello_world = cmake::project("Hello world");

    list<string> hw_sources = { "main.cpp" };
    hello_world.add_executable("hw_exec", hw_sources);

    return 0;
}
```

Here, we ensure that we run a cmake version that supports CMakeSL.
Then, we create a `hello_world` variable of type `project`. In the `project`'s constructor,
a project with name `"Hello world"` is registered.
Then, a list with source files is created and passed to `project::add_executable` method.
This method registers an executable with name `hw_exec` and the list of sources, containing only one file: `main.cpp`.
One more thing that `add_executable` does is that it returns a variable of type `executable`.
We don't need it, so we just don't assign it to anything.

The last thing that is done, is returning `0` from main.

## More complex example
Now, let's create an executable that links to a library which was created in a subdirectory.
Files structure:
```
├── CMakeLists.cmsl
├── lib
│   ├── CMakeLists.cmsl
│   └── lib.cpp
└── main.cpp
```

`CMakeLists.cmsl`:
```cpp
int main()
{
    cmake::minimum_required(version(3, 0, 0, 0));

    cmake::project hello_world = cmake::project("Hello world");

    auto lib = add_subdirectory("lib", hello_world);

    list<string> sources = {
        "main.cpp"
    };
    auto exec = hello_world.add_executable("hw_exec", sources);
    exec.link_to(lib);

    return 0;
}
```

`lib/CMakeLists.cmsl`:
```cpp
cmake::library main(cmake::project& top_lvl_project)
{
    list<string> sources = {
        "lib.cpp"
    };
    return top_lvl_project.add_library("hw_lib", sources);
}

```
