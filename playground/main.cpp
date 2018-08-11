#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

int main()
{
    const auto source = "int main()"
                        "{"
                        "    return 4+2;"
                        "}";

    cmsl::exec::executor exec;
    auto ret = exec.execute(source);
}
