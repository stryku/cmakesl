#include "lib/cmsl_complete.hpp"

#include <stdio.h>

int complete_and_print_results(struct cmsl_parsed_source* parsed_source, unsigned position)
{
    struct cmsl_complete_results* results = cmsl_complete_at(parsed_source, position);
    if(!results)
    {
        return 1;
    }

    int i;
    for(i = 0; i < results->num_results; ++i)
    {
        printf("%s\n", results->results[i]);
    }
    cmsl_destroy_complete_results(results);

    return 0;
}

int main()
{
    const char* source = "class hello_complete_world {}; int foo() {} int bar;  int main(int param_1, list<string> param_2) {  } int baz() {} class last_class {  };";

    struct cmsl_parsed_source* parsed_source = cmsl_parse_source(source);
    if(!parsed_source)
    {
        return 1;
    }

    const unsigned before_main_declaration_pos = 54u;
    const unsigned main_body_pos = 100u;
    const unsigned last_class_body_pos = 135u;

    printf("Completion just before the main function: \n");
    if(complete_and_print_results(parsed_source, before_main_declaration_pos) != 0)
    {
        cmsl_destroy_parsed_source(parsed_source);
        return 2;
    }

    printf("\n\nCompletion inside the main function: \n");
    if(complete_and_print_results(parsed_source, main_body_pos) != 0)
    {
        cmsl_destroy_parsed_source(parsed_source);
        return 2;
    }

    printf("\n\nCompletion inside the last_class body: \n");
    if(complete_and_print_results(parsed_source, last_class_body_pos) != 0)
    {
        cmsl_destroy_parsed_source(parsed_source);
        return 2;
    }

    cmsl_destroy_parsed_source(parsed_source);

    return 0;
}
