#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "int foo(int a, int b)"
        "{"
        "    return a + b;"
        "}"
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
