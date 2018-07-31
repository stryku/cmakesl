#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

int main()
{
    const auto source = "int main()"
                        "{"
                        "    4+2;"
                        "}";

    cmsl::exec::executor exec;
    exec.execute(source);
}
