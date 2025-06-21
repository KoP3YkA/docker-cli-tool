#include <string.h>
#include <stdbool.h>
#include "../../logic/create_files.h"
#include "../../files/file_manager.h"
#include "../../templates/template_manager.h"
#include "../../utils/string_util.h"

void set_params(int size, char* args[], char** version, bool* nasm, bool* node_build, bool* mysql, bool* force, bool* postgress, bool* mongo) {
    for (int i = 0; i < size; i++) {
        if (!strstartwith(args[i], "--")) continue;

        if (strstartwith(args[i], "--v")) {
            *version = args[i] + 3;
        } else if (strcmp(args[i], "--nasm") == 0) {
            *nasm = true;
        } else if (strcmp(args[i], "--nodeBuild") == 0) {
            *node_build = true;
        } else if (strcmp(args[i], "--mysql") == 0) {
            *mysql = true;
        } else if (strcmp(args[i], "--force") == 0) {
            *force = true;
        } else if (strcmp(args[i], "--postgre") == 0) {
            *postgress = true;
        } else if (strcmp(args[i], "--mongodb") == 0) {
            *mongo = true;
        }

    }
}

char* handle_template(char* name, int argc, char *argv[]) {
    char* template_path = get_template(name);
    if (!template_path) return NULL;

    ManagerFile template_file = { template_path };
    char* template_text = file_read(&template_file);
    if (!template_text) return NULL;

    Template template = { template_path, template_text };
    char* res = render_template(&template, argc, argv);

    if (!res) return "Success!";
    return res;
}

char* generate(int size, char *argv[]) {
    if (size == 0) return "You dont type any args";

    char* is_template = handle_template(argv[0], size, argv);
    if (is_template) return is_template;

    char* version = NULL;
    bool nasm = false;
    bool node_build = false;
    bool mysql = false;
    bool force = false;
    bool postgress = false;
    bool mongo = false;

    set_params(size, argv, &version, &nasm, &node_build, &mysql, &force, &postgress, &mongo);
    char* res = create_files(argv, version, nasm, node_build, mysql, force, postgress, mongo);

    if (!res) return "Success!";
    return res;
}