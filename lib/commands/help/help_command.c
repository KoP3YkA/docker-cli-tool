#include <stdio.h>
#include "../../globals.h"

void help() {
    printf("DockerCLI v%s\n\nhelp -- list of commands\ngenerate [from] (--options)\ntemplate [create/delete] [name]\n", version);
}