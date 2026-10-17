#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_json_field(const char *json_str, const char *key, char *dest, size_t dest_size) {
    if (!json_str || !key || !dest) return 0;

    char search_pattern[128];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\"", key);

    char *ptr = strstr(json_str, search_pattern);
    if (!ptr) return 0;

    // Move past the key and look for colon ':'
    ptr += strlen(search_pattern);
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r' || *ptr == ':') {
        ptr++;
    }

    // Check if the value is a string enclosed in quotes
    if (*ptr == '"') {
        ptr++; // Skip opening quote
        size_t i = 0;
        while (*ptr && *ptr != '"' && i < dest_size - 1) {
            dest[i++] = *ptr++;
        }
        dest[i] = '\0';
        return 1;
    }

    return 0;
}