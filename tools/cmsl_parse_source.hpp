#ifndef CMSL_PARSE_SOURCE_HPP
#define CMSL_PARSE_SOURCE_HPP

extern "C"
{
struct cmsl_parsed_source;

cmsl_parsed_source *cmsl_parse_source(const char *source);
void cmsl_destroy_parsed_source(cmsl_parsed_source *parsed_source);
}

#endif //CMSL_PARSE_SOURCE_HPP
