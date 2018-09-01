#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

#include <iostream>

int main()
{
    const auto source =
        "int main()"
        "{"
        "    string s = \"test\";"
        "    return s.size();"
        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
