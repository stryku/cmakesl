#ifndef CMSL_PARSE_SOURCE_HPP
#define CMSL_PARSE_SOURCE_HPP

#ifdef __cplusplus
extern "C" {
#endif

struct cmsl_parsed_source;

struct cmsl_parsed_source* cmsl_parse_source(const char* source);
void cmsl_destroy_parsed_source(struct cmsl_parsed_source* parsed_source);

#ifdef __cplusplus
}
#endif

#endif // CMSL_PARSE_SOURCE_HPP
