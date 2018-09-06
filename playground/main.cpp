#include "lexer/lexer.hpp"
#include "exec/source_executor.hpp"

#include <iostream>

int main()
{
    const auto source =
        "int main()"
        "{"
        "    list<string> l;"
        "    l.push_back(\"1\");"
        "    return l.size();"
        "}";

    cmsl::exec::source_executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
