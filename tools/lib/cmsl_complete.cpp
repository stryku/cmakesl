#include "cmsl_complete.hpp"
#include "cmsl_parsed_source.hpp"
#include "completer.hpp"


cmsl_complete_results* cmsl_complete_at(const cmsl_parsed_source* parsed_source, unsigned absolute_position)
{
    return cmsl::tools::completer{ *parsed_source, absolute_position }.complete();
}

void cmsl_destroy_complete_results(cmsl_complete_results* complete_results)
{
    for(auto i = 0u; i < complete_results->num_results; ++i)
    {
        delete [] complete_results->results[i];
    }

    delete [] complete_results->results;
    delete complete_results;
}

