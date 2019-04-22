#include "cmsl_index.hpp"

#include <stdio.h>

int main()
{
    const char* source = "class hello_complete_world {};"
                         "int foo() {}"
                         "int bar;"
                         "int main(int param_1, list<string> param_2) {}"
                         "int baz() {}"
                         "class last_class {};";

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
        printf("source begin: %d, source end: %d, entry type: %d, destination position: %d\n", entry->begin_pos, entry->end_pos, entry->type, entry->position);
    }

    cmsl_destroy_index_entries(index_entries);
    cmsl_destroy_parsed_source(parsed_source);

    return 0;
}
