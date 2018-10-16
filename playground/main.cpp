#include "lexer/lexer.hpp"
#include "exec/source_executor.hpp"
#include "cmake_facade.hpp"

#include <iostream>

class fake_facade : public cmsl::facade::cmake_facade
{
public:
    version get_cmake_version() const override
    {
        return {};
    }
};

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

    fake_facade facade{};
    cmsl::exec::source_executor exec{ facade };
    auto ret = exec.execute(source);
    std::cout << ret << '\n';
}
