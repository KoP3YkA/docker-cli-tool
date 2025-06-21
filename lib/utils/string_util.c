#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool strstartwith(const char *str, const char *prefix) {
    size_t len_prefix = strlen(prefix);
    return strncmp(str, prefix, len_prefix) == 0;
}

char* str_replace(const char* str, const char* old_sub, const char* new_sub) {
    if (!str || !old_sub || !new_sub) return NULL;

    size_t len_old = strlen(old_sub);
    size_t len_new = strlen(new_sub);

    int count = 0;
    const char* tmp = str;
    while ((tmp = strstr(tmp, old_sub))) {
        count++;
        tmp += len_old;
    }

    size_t new_len = strlen(str) + count * (len_new - len_old) + 1;
    char* result = malloc(new_len);
    if (!result) return NULL;

    char* dst = result;
    const char* current = str;

    while ((tmp = strstr(current, old_sub))) {
        size_t len = tmp - current;
        memcpy(dst, current, len);
        dst += len;

        memcpy(dst, new_sub, len_new);
        dst += len_new;

        current = tmp + len_old;
    }

    strcpy(dst, current);

    return result;
}