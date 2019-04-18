#include "cmsl_complete.hpp"

#include <iostream>

int main()
{
    const auto source = "class hello_complete_world {}; int foo() {} int bar;  int main(int param_1, list<string> param_2) {  } int baz() {} class last_class {  };";

    auto parsed_source = cmsl_parse_source(source);
    if(!parsed_source)
    {
        return 1;
    }

    const auto before_main_declaration_pos = 54u;
    const auto main_body_pos = 100u;
    const auto last_class_body_pos = 135u;
    const auto results = cmsl_complete_at(parsed_source, before_main_declaration_pos);
    if(!results)
    {
        return 2;
    }

    for(auto i = 0; i < results->num_results; ++i)
    {
        std::cout << results->results[i] << '\n';
    }

    cmsl_destroy_complete_results(results);
    cmsl_destroy_parsed_source(parsed_source);

    return 0;
}
