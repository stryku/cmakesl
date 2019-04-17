#include "cmsl_complete.hpp"

#include <iostream>

int main()
{
    const auto source = "class hello_complete_world {};"
                        "int foo() {}"
                        "int bar;"
                        "int main(int param_1, list<string> param_2) {  }"
                        "int baz() {}"
                        "class not_counted {};";

    auto parsed_source = cmsl_parse_source(source);
    if(!parsed_source)
    {
        return 1;
    }

    const auto results = cmsl_complete_at(parsed_source, 97u);
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
