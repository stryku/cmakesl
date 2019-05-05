#pragma once

#include "cmsl_parse_source.hpp"

#ifdef __cplusplus
extern "C" {
#endif
struct cmsl_complete_results
{
  char** results;
  unsigned num_results;
};

struct cmsl_complete_results* cmsl_complete_at(
  const struct cmsl_parsed_source* parsed_source, unsigned absolute_position);
void cmsl_destroy_complete_results(
  struct cmsl_complete_results* complete_results);

#ifdef __cplusplus
}
#endif
