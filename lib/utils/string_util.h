#ifndef DOCKER_CLI_TOOL_STRING_UTIL_H
#define DOCKER_CLI_TOOL_STRING_UTIL_H

#include <stdbool.h>

bool strstartwith(const char *str, const char *prefix);
char* str_replace(const char* str, const char* old_sub, const char* new_sub);

#endif //DOCKER_CLI_TOOL_STRING_UTIL_H
