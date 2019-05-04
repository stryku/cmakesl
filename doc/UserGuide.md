# CMakeSL User guide

## CMakeLists.txt vs CMakeLists.cmsl
CMake binary expects that CMakeSL scripts are named `CMakeLists.cmsl`. Based on extension it knows how to execute the scripts.

## Entry point of scripts execution
Every `CMakeLists.cmsl` script has to have a main function. It's the entry point of script execution.

The main function of the top-level script has to have signature taking no parameters and returning an int:
```cpp
int main() { ... }
```

Main function of scripts in subdirectories are allowed to have any signature, but the function has to be named `main`. E.g.:
```cpp
library main(project& top_lvl_project, string some_additional_parameter)
{
    ...
}
```

Script is allowed to have one, and exactly one, main function.

## Builtin types
Here's a list of currently implemented builtin types:
* bool
* int
* double
* string
* version
* list<T>
* project
* executable
* library

Here's a list of currently implemented builtin functions:
* cmake_minimum_required

For documentation, please refer to `doc/builtin_types`


## Hello world
CMakeSL doesn't provide any way to print anything on the screen, so the hello world is not very impressive.
```cpp
int main()
{
    return 0;
}
```

That's the simplest CMakeSL script.

Here's one that actually does something useful.
```cpp
int main()
{
    cmake_minimum_required(version(3,14,3))

    project hello_world = project("Hello world");

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
    cmake_minimum_required(version(3, 0, 0, 0));

    project hello_world = project("Hello world");

    library lib = add_subdirectory("lib", hello_world);

    list<string> sources = {
        "main.cpp"
    };
    executable exec = hello_world.add_executable("hw_exec", sources);
    exec.link_to(lib);

    return 0;
}
```

`lib/CMakeLists.cmsl`:
```cpp
library main(project& top_lvl_project)
{
    list<string> sources = {
        "lib.cpp"
    };
    return top_lvl_project.add_library("hw_lib", sources);
}

```

## Builtin types
All types are classes, even the primitives (bool, int, double).
For a full documentation, please refer to `doc/builtin_types`.

### bool
Not so much to say. A bool type that does what you expect from bool.
It's a class, so you can e.g. call `to_string()` method:
```cpp
bool foo = true;
string bar = foo.to_string();
```

### int
A signed 64 bit int. Has `to_string`, as well:
```cpp
int foo = 42;
string bar = foo.to_string();
```

### double
A typical double. Not much to say.

### string
A string type implementing basic operations from C++' `std::string` and some from the one from Python.

### version
A type representing version. You passes it to `cmake_minimum_required`:
```cpp
version ver = version(3, 14);
cmake_minimum_required(ver);
string str_version = ver.to_string();
```

### list
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

### project
Registers a project in the build system. It registers it only at `project("some name")` constructor, so you can copy the variable around, without duplicating more projects.
```cpp
project foo = project("The Foo");
foo.add_executable(...);
```

### library
Represents library. `project::add_library` returns an instance of it, so other libraries and executables can link to it.
```cpp
project foo = projecr("The Foo");

library bar_lib = foo.add_library("bar_lib", { "bar.cpp" });
library baz_lib = foo.add_library("baz_lib, { "baz.cpp" });

baz_lib.link_to(bar_lib);
```

### executable
Similar to library but other executables and libraries can't link to it (it's actually ensured by type safety - `library/executable::link_to` accepts only instances of the `library` type):
```cpp
project foo = projecr("The Foo");

library bar_lib = foo.add_library("bar_lib", { "bar.cpp" });
library baz_exec = foo.add_executable("baz_exec", { "baz.cpp" });

baz_exec.link_to(bar_lib);
```


