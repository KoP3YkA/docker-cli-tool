#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../files/file_manager.h"

void create_dockerfile(char* args[], char* version, bool nasm, bool node_build) {
    ManagerFile dockerfile = { "Dockerfile" };
    create_file_if_not_exists(&dockerfile);

    const char *from = args[0];
    const char *tag = version ? version : "latest";

    char line[256];
    snprintf(line, sizeof(line), "FROM %s:%s", from, tag);
    overwrite_file(&dockerfile, line);

    if (nasm) {
        add_text_to_file(&dockerfile, "\n\nRUN apt update && apt install -y nasm binutils");
    }

    add_text_to_file(&dockerfile, "\n\nWORKDIR /app");

    if (strcmp(from, "node") == 0) {
        add_text_to_file(&dockerfile, "\n\nCOPY package.json package-lock.json* ./");
        add_text_to_file(&dockerfile, "\n\nRUN npm install");
    }

    add_text_to_file(&dockerfile, "\n\nCOPY . .");

    if (strcmp(from, "python") == 0) {
        add_text_to_file(&dockerfile, "\n\nRUN pip install");
        add_text_to_file(&dockerfile, "\n\nCMD [\"python3\", \"main.py\"]");
    }
    else if (strcmp(from, "node") == 0) {
        if (node_build) {
            add_text_to_file(&dockerfile, "\n\nRUN npm run build");
            add_text_to_file(&dockerfile, "\n\nCMD [\"node\", \"dist/main.js\"]");
        }
        else add_text_to_file(&dockerfile, "\n\nCMD [\"node\", \"main.js\"]");
    }
    else if (strcmp(from, "gcc") == 0) {
        if (nasm) {
            add_text_to_file(&dockerfile,"\n\nRUN gcc main.c -o app");
            add_text_to_file(&dockerfile, "\n\nCMD [\"/bin/bash\", \"-c\", \"./app && nasm -f elf64 output.asm -o output.o && ld output.o -o output && ./output\"]");
        }
        else {
            add_text_to_file(&dockerfile, "\n\nRUN gcc main.c -o main");
            add_text_to_file(&dockerfile, "\n\nCMD [\"./main\"]");
        }
    } else if (nasm) {
        add_text_to_file(&dockerfile, "\n\nRUN nasm -f elf64 main.asm -o main.o");
        add_text_to_file(&dockerfile, "\n\nRUN ld main.o -o main");
        add_text_to_file(&dockerfile, "\n\nCMD [\"./main\"]");
    }
}

void create_compose(bool mysql, bool postgress, bool mongo) {
    ManagerFile compose = { "docker-compose.yml" };

    create_file_if_not_exists(&compose);

    if (mysql) {
        overwrite_file(&compose, "version: '3.8'");
        add_text_to_file(&compose, "\n\nservices:");
        add_text_to_file(&compose, "\n  mysql:");
        add_text_to_file(&compose, "\n    image: mysql:8.0");
        add_text_to_file(&compose, "\n    command: --log-error-verbosity=2");
        add_text_to_file(&compose, "\n    restart: always");
        add_text_to_file(&compose, "\n    environment:");
        add_text_to_file(&compose, "\n      MYSQL_ROOT_PASSWORD: passroot");
        add_text_to_file(&compose, "\n      MYSQL_USER: msqluser");
        add_text_to_file(&compose, "\n      MYSQL_PASSWORD: msqlpass");
        add_text_to_file(&compose, "\n      MYSQL_DATABASE: app");
        add_text_to_file(&compose, "\n    ports:");
        add_text_to_file(&compose, "\n      - \"3306:3306\"");
        add_text_to_file(&compose, "\n    volumes:");
        add_text_to_file(&compose, "\n      - mysql_data:/var/lib/mysql");
        add_text_to_file(&compose, "\n    healthcheck:");
        add_text_to_file(&compose, "\n      test: [ \"CMD\", \"mysqladmin\", \"ping\", \"-h\", \"localhost\" ]");
        add_text_to_file(&compose, "\n      interval: 5s");
        add_text_to_file(&compose, "\n      timeout: 10s");
        add_text_to_file(&compose, "\n      retries: 5");
        add_text_to_file(&compose, "\n\n  app:");
        add_text_to_file(&compose, "\n    depends_on:");
        add_text_to_file(&compose, "\n      mysql:");
        add_text_to_file(&compose, "\n        condition: service_healthy");
        add_text_to_file(&compose, "\n    build: .");
        add_text_to_file(&compose, "\n    tty: true");
        add_text_to_file(&compose, "\n\nvolumes:");
        add_text_to_file(&compose, "\n  mysql_data:");
    }
    else if (postgress) {
        overwrite_file(&compose, "version: '3.8'");
        add_text_to_file(&compose, "\n\nservices:");
        add_text_to_file(&compose, "\n  postgres:");
        add_text_to_file(&compose, "\n    image: postgres:15");
        add_text_to_file(&compose, "\n    restart: always");
        add_text_to_file(&compose, "\n    environment:");
        add_text_to_file(&compose, "\n      POSTGRES_USER: pguser");
        add_text_to_file(&compose, "\n      POSTGRES_PASSWORD: pgpass");
        add_text_to_file(&compose, "\n      POSTGRES_DB: appdb");
        add_text_to_file(&compose, "\n    ports:");
        add_text_to_file(&compose, "\n      - \"5432:5432\"");
        add_text_to_file(&compose, "\n    volumes:");
        add_text_to_file(&compose, "\n      - postgres_data:/var/lib/postgresql/data");
        add_text_to_file(&compose, "\n    healthcheck:");
        add_text_to_file(&compose, "\n      test: [\"CMD-SHELL\", \"pg_isready -U pguser\"]");
        add_text_to_file(&compose, "\n      interval: 5s");
        add_text_to_file(&compose, "\n      timeout: 10s");
        add_text_to_file(&compose, "\n      retries: 5");
        add_text_to_file(&compose, "\n\n  app:");
        add_text_to_file(&compose, "\n    depends_on:");
        add_text_to_file(&compose, "\n      postgres:");
        add_text_to_file(&compose, "\n        condition: service_healthy");
        add_text_to_file(&compose, "\n    build: .");
        add_text_to_file(&compose, "\n    tty: true");
        add_text_to_file(&compose, "\n\nvolumes:");
        add_text_to_file(&compose, "\n  postgres_data:");
    }
    else if (mongo) {
        overwrite_file(&compose, "version: '3.8'");
        add_text_to_file(&compose, "\n\nservices:");
        add_text_to_file(&compose, "\n  mongo:");
        add_text_to_file(&compose, "\n    image: mongo:7.0");
        add_text_to_file(&compose, "\n    restart: always");
        add_text_to_file(&compose, "\n    environment:");
        add_text_to_file(&compose, "\n      MONGO_INITDB_ROOT_USERNAME: mongoadmin");
        add_text_to_file(&compose, "\n      MONGO_INITDB_ROOT_PASSWORD: mongopass");
        add_text_to_file(&compose, "\n    ports:");
        add_text_to_file(&compose, "\n      - \"27017:27017\"");
        add_text_to_file(&compose, "\n    volumes:");
        add_text_to_file(&compose, "\n      - mongo_data:/data/db");
        add_text_to_file(&compose, "\n    healthcheck:");
        add_text_to_file(&compose, "\n      test: [\"CMD\", \"mongosh\", \"--eval\", \"db.adminCommand('ping')\"]");
        add_text_to_file(&compose, "\n      interval: 5s");
        add_text_to_file(&compose, "\n      timeout: 10s");
        add_text_to_file(&compose, "\n      retries: 5");
        add_text_to_file(&compose, "\n\n  app:");
        add_text_to_file(&compose, "\n    depends_on:");
        add_text_to_file(&compose, "\n      mongo:");
        add_text_to_file(&compose, "\n        condition: service_healthy");
        add_text_to_file(&compose, "\n    build: .");
        add_text_to_file(&compose, "\n    tty: true");
        add_text_to_file(&compose, "\n\nvolumes:");
        add_text_to_file(&compose, "\n  mongo_data:");
    }
    else {
        overwrite_file(&compose, "version: '3.8'");
        add_text_to_file(&compose, "\n\nservices:");
        add_text_to_file(&compose, "\n  app:");
        add_text_to_file(&compose, "\n    build: .");
        add_text_to_file(&compose, "\n    tty: true");
    }
}

char* create_files(char* args[], char* version, bool nasm, bool node_build, bool mysql, bool force, bool postgress, bool mongo) {
    ManagerFile dockerfile = { "Dockerfile" };
    char* read_dockerfile = file_read(&dockerfile);

    ManagerFile docker_compose = { "docker-compose.yml" };
    char* read_compose = file_read(&docker_compose);

    if (!force && (read_compose != NULL || read_dockerfile != NULL)) return "Dockerfile or docker-compose.yml already exists! Use --force for overwrite";

    create_dockerfile(args, version, nasm, node_build);
    create_compose(mysql, postgress, mongo);

    return NULL;
}