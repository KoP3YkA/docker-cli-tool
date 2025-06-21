#include <malloc.h>
#include "stdio.h"
#include <stdlib.h>
#include <fileapi.h>
#include <errhandlingapi.h>
#include <winerror.h>

typedef struct {
    char* path;
} ManagerFile;

char *file_read(ManagerFile* file) {
    FILE *readFile = fopen(file->path, "r");
    if (readFile == NULL) {
        return NULL;
    }

    fseek(readFile, 0, SEEK_END);
    long filesize = ftell(readFile);
    rewind(readFile);

    char *buffer = malloc(filesize + 1);
    if (!buffer) {
        fclose(readFile);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, filesize, readFile);
    buffer[read_size] = '\0';

    fclose(readFile);
    return buffer;
}

int create_directory_if_not_exists(ManagerFile* file) {
    if (CreateDirectory(file->path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }
    return 1;
}

int create_file_if_not_exists(ManagerFile* file) {
    FILE *newFile = fopen(file->path, "w");
    if (!newFile) return 1;
    fclose(newFile);
    return 0;
}

int overwrite_file(ManagerFile* file, char* text) {
    FILE *newFile = fopen(file->path, "w");
    if (!newFile) return 1;
    fprintf(newFile, "%s", text);
    fclose(newFile);
    return 0;
}

int add_text_to_file(ManagerFile* file, char* text) {
    FILE *newFile = fopen(file->path, "a");
    if (!newFile) return 1;
    fprintf(newFile, "%s", text);
    fclose(newFile);
    return 0;
}

int delete_file(ManagerFile* file) {
    if (DeleteFileA(file->path)) return 0;
    else return 1;
}
