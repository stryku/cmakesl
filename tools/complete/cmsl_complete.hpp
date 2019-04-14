#pragma once

extern "C"
{
    struct cmsl_parsed_source;

    cmsl_parsed_source* cmsl_parse_source(const char* source);
    void cmsl_destroy_parsed_source(cmsl_parsed_source* parsed_source);

    struct cmsl_complete_results
    {
        const char ** results;
        unsigned num_results;
    };

    cmsl_complete_results* cmsl_complete_at(const cmsl_parsed_source* parsed_source, unsigned absolute_position);
    void cmsl_destroy_complete_results(cmsl_complete_results* complete_results);
}
