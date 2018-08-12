#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source = "int main()"
                        "{"
                        "    int a = 4+2;"
                        "    return a + 10 - (40 + 2);"
                        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
