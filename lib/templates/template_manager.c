#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "../json/cJSON.h"
#include "../utils/string_util.h"
#include "../files/file_manager.h"

typedef struct {
    char* path;
    char* text;
} Template;

char* get_template(char* path) {
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    char* last_slash = strrchr(exe_path, '\\');
    if (!last_slash) return NULL;
    *(last_slash + 1) = '\0';

    static char full_path[MAX_PATH];
    sprintf(full_path, "%stemplates\\%s.json", exe_path, path);
    return full_path;
}

char* get_templates_path() {
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    char* last_slash = strrchr(exe_path, '\\');
    if (!last_slash) return NULL;
    *(last_slash + 1) = '\0';

    static char full_path[MAX_PATH];
    sprintf(full_path, "%stemplates", exe_path);
    return full_path;
}

typedef struct {
    char* key;
    char* value;
} Variable;

char* get_file_text(cJSON* json, int var_index, Variable* variables, char jsonObject[], char* first_arg) {
    cJSON* json_dockerfile = cJSON_GetObjectItemCaseSensitive(json, jsonObject);
    if (!json_dockerfile || !cJSON_IsArray(json_dockerfile)) return "Cannot find object or it is not valid array";

    size_t total_length = 1;
    int string_len = 0;
    cJSON* json_temp_dockerfile_string = NULL;
    cJSON_ArrayForEach(json_temp_dockerfile_string, json_dockerfile) {
        if (!cJSON_IsString(json_temp_dockerfile_string)) return "All elements of object must be string";
        total_length += strlen(json_temp_dockerfile_string->valuestring);
        total_length++;
        string_len++;
    }

    int var_index_two = var_index;

    for (int i = 0; i < var_index; i++) {
        Variable var = variables[i];
        total_length += strlen(var.value) * string_len;
    }

    if (first_arg) total_length += strlen(first_arg);

    char* dockerfile_text = malloc(total_length);
    if (!dockerfile_text) return "Malloc returned NULL. Sorry :(";
    dockerfile_text[0] = '\0';

    if (first_arg) {
        strcat(dockerfile_text, first_arg);
        strcat(dockerfile_text, "\n");
    }

    cJSON_ArrayForEach(json_temp_dockerfile_string, json_dockerfile) {
        const char* orig_text = json_temp_dockerfile_string->valuestring;
        char* mod_text = strdup(orig_text);

        for (int i = 0; i < var_index_two; i++) {
            Variable var = variables[i];
            char val_to_replace[1024];
            sprintf(val_to_replace, "${%s}", var.key);
            char* replaced = str_replace(mod_text, val_to_replace, var.value);
            free(mod_text);
            mod_text = replaced;
        }

        strcat(dockerfile_text, mod_text);
        strcat(dockerfile_text, "\n");
        free(mod_text);
    }

    return dockerfile_text;
}

char* get_version(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strstartwith(argv[i], "--v")) {
            return argv[i] + 3;
        }
    }
    char* lasted = malloc(7);
    strcpy(lasted, "lasted");
    return lasted;
}

char* get_text_or_array_joined(cJSON* item) {
    if (!item) return NULL;

    if (cJSON_IsString(item)) {
        return strdup(item->valuestring);
    }
    else if (cJSON_IsArray(item)) {
        size_t total_len = 0;
        int count = 0;
        cJSON* element = NULL;

        cJSON_ArrayForEach(element, item) {
            if (!cJSON_IsString(element)) return NULL;
            total_len += strlen(element->valuestring);
            count++;
        }
        total_len += count;

        char* result = malloc(total_len);
        if (!result) return NULL;
        result[0] = '\0';

        int first = 1;
        cJSON_ArrayForEach(element, item) {
            if (!first) strcat(result, "\n");
            strcat(result, element->valuestring);
            first = 0;
        }

        return result;
    }
    else {
        return NULL;
    }
}

char* render_template(Template* template, int argc, char *argv[]) {
    cJSON* json = cJSON_Parse(template->text);
    if (!json) return "Cannot parse JSON. Check is template invalid";

    cJSON* json_from = cJSON_GetObjectItemCaseSensitive(json, "from");
    if (!json_from || !cJSON_IsString(json_from)) return "Key 'from' is not exists or invalid";

    char* dockerfile_from = json_from->valuestring;
    char buffer[256];
    strcpy(buffer, dockerfile_from);
    sprintf(dockerfile_from, "FROM %s:%s", buffer, get_version(argc, argv));

    cJSON* json_options = cJSON_GetObjectItemCaseSensitive(json, "options");
    if (!json_options || !cJSON_IsObject(json_options)) return "Key 'options' is not exists or invalid";

    Variable* variables = calloc(255, sizeof(Variable));
    if (!variables) return "Calloc returned NULL. Sorry :(";
    int var_index = 0;

    cJSON* json_variables = cJSON_GetObjectItemCaseSensitive(json, "variables");
    if (!json_variables || !cJSON_IsObject(json_variables)) return "Key 'variables' is not exists or invalid";

    bool is_force = false;

    cJSON* json_temp_option = NULL;
    cJSON_ArrayForEach(json_temp_option, json_options) {
        char* temp_value = json_temp_option->valuestring;
        char* temp_option = json_temp_option->string;

        bool exists = false;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], temp_option) == 0) exists = true;
            else if (strcmp(argv[i], "--force") == 0) is_force = true;
        }

        cJSON* json_temp_variable = NULL;
        cJSON_ArrayForEach(json_temp_variable, json_variables) {
            if (!json_temp_variable || !cJSON_IsObject(json_temp_variable)) return "Some variable is not exists or invalid";
            if (strcmp(json_temp_variable->string, temp_value) != 0) continue;

            cJSON* true_item = cJSON_GetObjectItemCaseSensitive(json_temp_variable, "true");
            cJSON* false_item = cJSON_GetObjectItemCaseSensitive(json_temp_variable, "false");

            char* if_is_true = get_text_or_array_joined(true_item);
            char* if_is_false = get_text_or_array_joined(false_item);

            if (!if_is_true || !if_is_false) {
                free(if_is_true);
                free(if_is_false);
                return "In some variable `true` or `false` is not specified or invalid";
            }

            variables[var_index].key = strdup(json_temp_variable->string);
            if (exists) variables[var_index].value = strdup(if_is_true);
            else variables[var_index].value = strdup(if_is_false);

            var_index++;
        }
    }

    char* dockerfile_text = get_file_text(json, var_index, variables, "dockerfile", dockerfile_from);

    ManagerFile dockerfile_file = { "Dockerfile" };
    char* dockerfile_file_text = file_read(&dockerfile_file);
    if (dockerfile_file_text && !is_force) return "Dockerfile already exists! Use --force";
    overwrite_file(&dockerfile_file, dockerfile_text);
    free(dockerfile_text);

    char* docker_compose_text = get_file_text(json, var_index, variables, "compose", NULL);

    ManagerFile docker_compose_file = {"docker-compose.yml" };
    char* docker_compose_file_text = file_read(&docker_compose_file);
    if (docker_compose_file_text && !is_force) return "docker-compose.yml already exists! Use --force";
    overwrite_file(&docker_compose_file, docker_compose_text);
    free(docker_compose_text);

    cJSON_Delete(json);
    free(variables);
    return NULL;
}