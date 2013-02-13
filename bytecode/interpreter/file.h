#ifndef BYTECODE_FILE_H
#define BYTECODE_FILE_H
#include <stdint.h>
#include <stdbool.h>
typedef uint64_t location_t;

#if DEBUG
void *relocate(location_t offset);
#else
extern char *file_start;
#define relocate(off) ((void*)(file_start+off))
#endif

location_t derelocate(void *ptr);

void load_file(const char *filename);

const char *get_location_name(location_t location);
const char *try_translate_into_name(void *ptr);
#endif
