#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "int foo()"
        "{"
        "    int a = 4;"
        "    int b = 16;"
        "    return a + b;"
        "}"
        ""
        "int main()"
        "{"
        "    int a = 4+2;"
        "    return a + foo();"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
