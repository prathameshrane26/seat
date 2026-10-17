#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

// Simple JSON field extraction helper for parsing incoming HTTP request bodies (e.g., username, password, email)
int parse_json_field(const char *json_str, const char *key, char *dest, size_t dest_size);

#endif // PARSER_H