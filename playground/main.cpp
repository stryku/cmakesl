#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "class cl_foo"
        "{"
        "    int foo_member = 5;"
        "    int foo_member_2 = 30;"
        "};"
        ""
        "int main()"
        "{"
        "    int a = 4+2;"
        "    return a + foo(16, 4);"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
