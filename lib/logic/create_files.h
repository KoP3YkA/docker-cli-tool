#ifndef DOCKER_CLI_TOOL_CREATE_FILES_H
#define DOCKER_CLI_TOOL_CREATE_FILES_H
#include <stdbool.h>

char* create_files(char* args[], char* version, bool nasm, bool node_build, bool mysql, bool force, bool postgress, bool mongo);

#endif //DOCKER_CLI_TOOL_CREATE_FILES_H
