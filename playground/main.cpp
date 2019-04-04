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
    void fatal_error(const std::string& what) const override
    {}
    void register_project(const std::string& what) override
    {}


    std::string get_current_binary_dir() const override
    {
        return {};
    }
    std::string get_current_source_dir() const override
    {
        return {};
    }

    void add_executable(const std::string& name, const std::vector<std::string>& sources) override
    {}

    void add_library(const std::string& name, const std::vector<std::string>& sources) override
    {}

    void target_link_library(const std::string& , const std::string& ) override
    {}
    std::string current_directory() const
    {
        return "";
    }
    void go_into_subdirectory(const std::string& dir) {}
    void go_directory_up() {}
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
