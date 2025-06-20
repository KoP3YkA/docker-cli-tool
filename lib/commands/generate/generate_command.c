#include <string.h>
#include <stdbool.h>
#include "../../files/file_manager.h"
#include <stdio.h>
#include <malloc.h>

bool strstartwith(const char *str, const char *prefix) {
    size_t len_prefix = strlen(prefix);
    return strncmp(str, prefix, len_prefix) == 0;
}

char* generate(int size, char* args[]) {
    if (size == 0) return "You dont type any args";

    ManagerFile dockerfile = { "Dockerfile" };
    create_file_if_not_exists(&dockerfile);

    char* version = NULL;
    bool nasm = false;
    bool node_build = false;
    bool mysql = false;

    for (int i = 0; i < size; i++) {
        if (!strstartwith(args[i], "--")) continue;

        if (strstartwith(args[i], "--v")) {
            version = args[i] + 3;
        }

        if (strcmp(args[i], "--nasm") == 0) {
            nasm = true;
        }

        if (strcmp(args[i], "--nodeBuild") == 0) {
            node_build = true;
        }

        if (strcmp(args[i], "--mysql") == 0) {
            mysql = true;
        }

    }

    const char *from = args[0];
    const char *tag = version ? version : "latest";

    char line[256];
    snprintf(line, sizeof(line), "FROM %s:%s", from, tag);
    overwrite_file(&dockerfile, line);

    if (nasm) {
        add_text_to_file(&dockerfile, "\n\nRUN apt update && apt install -y nasm");
    }

    add_text_to_file(&dockerfile, "\n\nWORKDIR /app");

    if (strcmp(from, "node") == 0) {
        add_text_to_file(&dockerfile, "\n\nCOPY package.json package-lock.json* ./\n\nRUN npm install");
    }

    add_text_to_file(&dockerfile, "\n\nCOPY . .");

    if (strcmp(from, "python") == 0) {
        add_text_to_file(&dockerfile, "\n\nRUN pip install");
        add_text_to_file(&dockerfile, "\n\nCMD [\"python3\", \"main.py\"]");
    }
    else if (strcmp(from, "node") == 0) {
        if (node_build) add_text_to_file(&dockerfile, "\n\nRUN npm run build\n\nCMD [\"node\", \"dist/main.js\"]");
        else add_text_to_file(&dockerfile, "\n\nCMD [\"node\", \"main.js\"]");
    }
    else if (strcmp(from, "gcc") == 0) {
        if (nasm) add_text_to_file(&dockerfile, "\n\nRUN gcc main.c -o app\n\nCMD [\"/bin/bash\", \"-c\", \"./app && nasm -f elf64 output.asm -o output.o && ld output.o -o output && ./output\"]");
        else add_text_to_file(&dockerfile, "\n\nRUN gcc main.c -o main\n\nCMD [\"./main\"]");
    }

    ManagerFile compose = { "docker-compose.yml" };

    create_file_if_not_exists(&compose);

    if (!mysql) overwrite_file(&compose, "version: '3.8'\n\nservices:\n  app:\n    build: .\n    tty: true");
    else {
        overwrite_file(&compose, "version: '3.8'\n\nservices:\n  mysql:\n    image: mysql:8.0\n    command: --log-error-verbosity=2\n    restart: always\n    environment:\n      MYSQL_ROOT_PASSWORD: passroot\n      MYSQL_USER: msqluser\n      MYSQL_PASSWORD: msqlpass\n      MYSQL_DATABASE: app\n    ports:\n      - \"3306:3306\"\n    volumes:\n      - mysql_data:/var/lib/mysql\n    healthcheck:\n      test: [ \"CMD\", \"mysqladmin\", \"ping\", \"-h\", \"localhost\" ]\n      interval: 5s\n      timeout: 10s\n      retries: 5\n\n  app:\n    depends_on:\n      mysql:\n        condition: service_healthy\n    build: .\n    tty: true\n\nvolumes:\n  mysql_data:");
    }

    return "Success!";
}