#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

typedef struct {
    char* path;
} ManagerFile;

int create_directory_if_not_exists(ManagerFile* file);
char *file_read(ManagerFile* file);
int create_file_if_not_exists(ManagerFile* file);
int overwrite_file(ManagerFile* file, char* text);
int add_text_to_file(ManagerFile* file, char* text);
int delete_file(ManagerFile* file);

#endif
