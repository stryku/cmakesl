#include "lexer/lexer.hpp"
#include "exec/exec.hpp"

int main()
{
    const auto source = "4+2-(4+2)";

    cmsl::exec::executor exec;
    exec.execute(source);
}
