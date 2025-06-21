#include <stdio.h>
#include "lib/commands/help/help_command.h"
#include "lib/commands/generate/generate_command.h"
#include "lib/commands/template/template_command.h"
#include <string.h>

char version[] = "1.1";

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Welcome to DockerCLI v %s\nType <<help>> for list of commands\n", version);
        return 0;
    }

    char* current_command = argv[1];

    if (!current_command) {
        printf("Please, type the command\n");
        return 1;
    }

    if (strcmp(current_command, "help") == 0 || strcmp(current_command, "h") == 0) {
        help();
    }
    else if (strcmp(current_command, "generate") == 0 || strcmp(current_command, "g") == 0) {
        char* result = generate(argc - 2, argv + 2);
        printf(result);
    }
    else if (strcmp(current_command, "template") == 0 || strcmp(current_command, "temp") == 0) {
        template_command(argc - 2, argv + 2);
    }
    else {
        printf("Invalid command. Use <<help>> for list of commands\n");
        return 2;
    }

    return 0;
}
