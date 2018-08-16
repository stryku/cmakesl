#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "class cl_foo"
        "{"
        "    int foo_a = 5;"
        "    int foo_b = 30;"
        "};"
        ""
        "int main()"
        "{"
        "    int a = 4+2;"
        "    cl_foo foo;"
        "    return a + foo.a;"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
