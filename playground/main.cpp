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
        "    string s = \"str\""
        "    return s.size();"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
