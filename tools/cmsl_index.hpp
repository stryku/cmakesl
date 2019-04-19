#ifndef CMSL_INDEX_HPP
#define CMSL_INDEX_HPP

extern "C"
{
    enum cmsl_index_entry_type
    {
        type,
        function_call_name
    };

    struct cmsl_index_entry
    {
        unsigned begin_pos;
        unsigned end_pos;
        cmsl_index_entry_type type;
        char* source_path;
        unsigned position;
    };

struct cmsl_index_entries
{
    struct cmsl_index_entry* entries{ nullptr };
    unsigned num_entries{ 0u };
};

struct cmsl_index_entries* cmsl_index(const struct cmsl_parsed_source* parsed_source);
void cmsl_destroy_index_entries(struct cmsl_index_entries* index_entries);
}

#endif // CMSL_INDEX_HPP
