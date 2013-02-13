#ifndef BYTECODE_HASH_TABLE_H
#define BYTECODE_HASH_TABLE_H

#include <stdbool.h>
#include <stdlib.h>

/** Hash table of strings to void *. */
typedef struct {
    size_t   nmemb;
    size_t   alloc;
    struct {
	const char *restrict key;
	void *value;
    } *restrict arr;
} bytecode_hash_table;

void bytecode_hash_table_init(bytecode_hash_table *restrict h);
bool bytecode_hash_table_ins(bytecode_hash_table *restrict h,
			     const char *restrict key,
			     void *value);
void *bytecode_hash_table_get(const bytecode_hash_table *restrict h,
			      const char *restrict key);
void bytecode_hash_table_iter(bytecode_hash_table *restrict h,
			      void (*iterfunc)(void*,
					       const char*,
					       void*),
			      void *data);

#endif /*BYTECODE_HASH_TABLE_H*/
