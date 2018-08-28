#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "class cl_foo"
        "{"
        "    int sum(int param, int param2)"
        "    {"
        "        return foo_a + foo_b + param + param2;"
        "    }"
        ""
        "    int foo_a = 5;"
        "    int foo_b = 30;"
        "};"
        ""
        "int main()"
        "{"
        "    cl_foo foo;"
        "    int a = 4+2;"
        "    foo.foo_a = 14;"
        "    foo.foo_b = 100;"
        "    return a + foo.sum(a, a + foo.foo_a);"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
