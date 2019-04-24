#include "lib/cmsl_index.hpp"

#include <stdio.h>

const char* entry_type_to_string(enum cmsl_index_entry_type entry_type)
{
    switch(entry_type)
    {

        case type: return "type";
        case identifier: return "identifier";
        case parameter_declaration_identifier: return "parameter declaration identifier";
        case class_member_identifier: return "class member identifier";
        case operator_function: return "operator function";
        case function_call_name: return "function call name";
    }

    return "unknown";
}

int main()
{
    const char* source = "class hello_world"
                         "{"
                         "    int foo_getter()"
                         "    {"
                         "        return foo;"
                         "    }"
                         ""
                         "    int foo;"
                         "};"
                         ""
                         "int foo() { return 0; }"
                         ""
                         "int bar;"
                         ""
                         "int main(int param_1, list<string> param_2)"
                         "{"
                         "    hello_world hw;"
                         "    hw.foo = 42;"
                         "    return hw.foo_getter();"
                         "}";

    struct cmsl_parsed_source* parsed_source = cmsl_parse_source(source);
    if(!parsed_source)
    {
        return 1;
    }

    struct cmsl_index_entries* index_entries = cmsl_index(parsed_source);
    if(!index_entries)
    {
        cmsl_destroy_parsed_source(parsed_source);
        return 2;
    }

    int i;
    for(i = 0; i < index_entries->num_entries; ++i)
    {
        struct cmsl_index_entry* entry = &index_entries->entries[i];
        printf("source begin: %d, source end: %d, destination position: %d, entry type: %s\n", entry->begin_pos, entry->end_pos, entry->position, entry_type_to_string(entry->type));
    }

    cmsl_destroy_index_entries(index_entries);
    cmsl_destroy_parsed_source(parsed_source);

    return 0;
}
