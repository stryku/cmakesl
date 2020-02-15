# CMakeSL User guide

# Table of contents
- [CMakeSL User guide](#cmakesl-user-guide)
  - [CMakeLists.txt vs CMakeLists.cmsl](#cmakeliststxt-vs-cmakelistscmsl)
  - [Entry point of scripts execution](#entry-point-of-scripts-execution)
- [Builtin types](#builtin-types)
  - [void](#void)
  - [bool](#bool)
  - [int](#int)
  - [double](#double)
  - [string](#string)
  - [extern<T>](#externt)
  - [list<T>](#listt)
  - [cmake::option](#cmakeoption)
  - [cmake::version](#cmakeversion)
  - [project](#project)
  - [library](#library)
  - [executable](#executable)
- [User types](#user-types)
- [Type deduction](#type-deduction)
- [Designated initializers](#designated-initializers)
- [Import/export](#importexport)
- [Mixing CMakeSL with 'old' CMake](#mixing-cmakesl-with-old-cmake)
- [Examples](#examples)
  - [Hello world](#hello-world)
  - [More complex example](#more-complex-example)
- [Declarative format](#declarative-format)
  - [Builtin components](#builtin-components)
  - [forwarding_lists properties](#forwardinglists-properties)
  - [Custom components](#custom-components)
  - [Modules](#modules)
  - [Accessing old-style CMake variables](#accessing-old-style-cmake-variables)
  - [Declarative root file](#declarative-root-file)


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

Please see [the root CMakeLists.cmsl](https://github.com/stryku/cmakesl/blob/master/CMakeLists.cmsl#L41). There is a call `add_subdirectory("external/googletest")` which adds the whole googletest library that is later on used in the CMakeSL tests.


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
    cmake::minimum_required(cmake::version(3, 14, 3))

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
    cmake::minimum_required(cmake::version(3, 14));

    auto hello_world = cmake::project("Hello world");

    auto lib = add_subdirectory("lib", hello_world);

    auto sources = { "main.cpp" };
    auto exec = hello_world.add_executable("hw_exec", sources);

    exec.link_to(lib);

    return 0;
}
```

`lib/CMakeLists.cmsl`:
```cpp
cmake::library main(cmake::project& top_lvl_project)
{
    auto sources = { "lib.cpp" };
    return top_lvl_project.add_library("hw_lib", sources);
}

```

# Declarative format
Declarative format can be used as leaf nodes of you build system.

## Builtin components
There are four builtin component types that you should be aware of:
* `static_library`
* `shared_library`
* `executable`
* `test_executable`

All of them derive from `product` component type. You can find available properties in the [docs](https://github.com/stryku/cmakesl/blob/master/doc/builtin/decl.cmsl#L31).

## forwarding_lists properties
You've probably spotted an usage of property like `files.public`. At this point you probably figured out what does it mean.

In CMake a lot of properties can be `PUBLIC`, `PRIVATE` or `INTERFACE`. You can add directories to an include path of a target, with `PRIVATE` keyword. That means they won't be forwarded.

The same functionality comes with properties of `product` component, that are of `forwarding_lists` type. `forwarding_lists` has three properties:
* `public`
* `private`
* `interface`

and you can access them and assign to them like you saw earlier, e.g.:
```
static_library {
    name = "foo"

    files.public = [
        "foo.cpp"
    ]

    include_dirs.public = [
        "public/include/dir"
    ]

    inculde_dirs.private = [
        "private/include/dir"
    ]

    dependencies.interface = [
        "bar_dependency"
    ]
}
```

And so on..

## Custom components
(Check out a complete example: [custom_component](https://github.com/stryku/cmakesl/tree/master/examples/custom_component))

There are of course cases, when you would want to create a custom component. E.g. you'd want to add a suffix to all of your libraries. You could, of course, add the suffix manually in every `static_library` declaration, but that's ugly. Instead, you can create a custom component that accumulates common functionality and properties. Later on it can be used to declare stuff.

Let's say that we want all of our libraries to have suffix "_my_fancy_lib" and include directory "my/fancy/dir". Let's declare a custom component out of it, that derives from the builtin `static_library` component:
```
component my_fancy_lib : static_library {
    name_suffix = "_my_fancy_lib"

    include_dirs.public = [
        "my/fancy/dir"
    ]
}
```

And now it can be used to declare our fancy lib:
```
my_fancy_lib {
    name = "foo"

    include_dirs.public += [
        "another/fancy/dir"
    ]
}
```

Mind the `+=` while adding include directory. Thanks to that, the list with `another/fancy/dir` will be appended to the list in component declaration. If you'd use a plain `=`, the list would be overriden.

## Modules
(Check out a complete example: [module_import](https://github.com/stryku/cmakesl/tree/master/examples/module_import))

Imperative CMakeSL as well as the declarative one has modules support. With the example above, you most likely would want to declare `my_fance_lib` component in a commonly accessible file and use it wherever you need.

Just create the file, let's name it `my_fancy_lib.dcmsl` and in import it the file in whchich you want to use it:
```
import "my_fancy_lib.dcmsl";

my_fancy_lib {
    name = "foo"

    include_dirs.public += [
        "another/fancy/dir"
    ]
}
```

## Accessing old-style CMake variables
(Check out a complete example: [cmake_variables_accessor](https://github.com/stryku/cmakesl/tree/master/examples/cmake_variables_accessor))

In a lot of cases you'll need to get a value of an old-style CMake variable. You can get it from an accessor named `cmake_variables`. CMakeSL is statically typed, so you need to provide information how the variable should be treated, using `as_bool`, `as_int`, `as_double`, `as_string` or `as_list`.

For example, let's say that you have a root CMakeLists written in the vanilla CMake:
`CMakeLists.txt`:
```
cmake_minimum_required(VERSION 3.14.3)

project("MyFancyProject")

set(MY_FANCY_SUFFIX "_my_fancy_suffix")

add_subdirectory(my_fancy_lib)
```

In the `my_fancy_lib` dir, you declare a library that gets its name suffix from the accessor:
`my_fancy_lib/CMakeLists.dcmsl`:
```
static_library {
    name = "my_fancy_lib"
    name_suffix = cmake_variables.MY_FANCY_SUFFIX.as_string

    ...
}
```

## Declarative root file
(Check out a complete example: [declarative_root_cmakelists](https://github.com/stryku/cmakesl/tree/master/examples/declarative_root_cmakelists))

Declarative file can be used as the root CMakeLists. So, if you don't need to do anything fancy in the project, or just want to quickly check something out, that's the way.
