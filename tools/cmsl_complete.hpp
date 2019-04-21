#pragma once

#include "cmsl_parse_source.hpp"

extern "C"
{
    struct cmsl_complete_results
    {
        char ** results{ nullptr };
        unsigned num_results{ 0u };
    };

    cmsl_complete_results* cmsl_complete_at(const cmsl_parsed_source* parsed_source, unsigned absolute_position);
    void cmsl_destroy_complete_results(cmsl_complete_results* complete_results);
}
