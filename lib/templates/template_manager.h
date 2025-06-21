#ifndef DOCKER_CLI_TOOL_TEMPLATE_MANAGER_H
#define DOCKER_CLI_TOOL_TEMPLATE_MANAGER_H

typedef struct {
    char* path;
    char* text;
} Template;

char* get_templates_path();
char* get_template(char* path);
char* render_template(Template* template, int argc, char *argv[]);

#endif //DOCKER_CLI_TOOL_TEMPLATE_MANAGER_H
