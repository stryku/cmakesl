#include "lexer/lexer.hpp"
#include "exec/source_executor.hpp"

#include <iostream>

int main()
{
    const auto source =
        "class Version"
        "{"
            "Version(int major, int minor, int patch)"
            "{"
                "this.major = major;"
                "this.minor = minor;"
                "this.patch = patch;"
            "}"
            ""
            "int major;"
            "int minor;"
            "int patch;"
        "};"
        ""
        "int main()"
        "{"
        "    Version v = Version( 1, 2, 3 );"
        "    return v.major + v.minor + v.patch;"
        "}";

    cmsl::exec::source_executor exec;
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
