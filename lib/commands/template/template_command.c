#include <stdio.h>
#include <string.h>
#include "../../files/file_manager.h"
#include "../../templates/template_manager.h"

void template_command(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Type create/delete and name of template.");
        return;
    }

    char* action = argv[0];
    char* name = argv[1];
    char buffer[36];
    strcpy(buffer, name);
    sprintf(name, "%s.json", buffer);

    if (strcmp(action, "create") != 0 && strcmp(action, "delete") != 0) {
        printf("Type create or delete");
        return;
    }

    if (strlen(name) > 32) {
        printf("Name must be < 32");
        return;
    }

    char* templates_directory_path = get_templates_path();
    if (!templates_directory_path) {
        printf("Cannot get templates directory.");
        return;
    }
    ManagerFile templates_directory = { templates_directory_path };
    create_directory_if_not_exists(&templates_directory);

    char* current_template_path = get_template(buffer);
    if (!current_template_path) {
        printf("Cannot get this path");
        return;
    }

    ManagerFile template = { current_template_path };

    if (strcmp(action, "create") == 0) {
        char* text = file_read(&template);
        if (text) {
            printf("This file is already exists.");
            return;
        }

        overwrite_file(&template, "{\n  \"from\": \"some\",\n  \"dockerfile\": [\n    \"WORKDIR /app\"\n  ],\n  \"variables\": {},\n  \"options\": {},\n  \"compose\": [\n    \"version: '3.8'\"\n  ]\n}");
        printf("Success created %s", template.path);
        return;
    } else {
        char* text = file_read(&template);
        if (!text) {
            printf("This file is not exists.");
            return;
        }
        delete_file(&template);
        printf("Success deleted %s", name);
        return;
    }
}