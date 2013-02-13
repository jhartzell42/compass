#ifndef BYTECODE_UTIL_H
#define BYTECODE_UTIL_H

#include <stdlib.h>
#include <stdio.h>

void *bytecode_malloc(size_t size);
void *bytecode_realloc(void *restrict p, size_t size);
char *bytecode_strdup(const char *in);
char *bytecode_afgets(FILE *stream);
void eat_whitespace(char **ptr);
void skip_identifier(char **ptr);
char *grab_identifier(char **ptr);

#endif /* BYTECODE_UTIL_H */
