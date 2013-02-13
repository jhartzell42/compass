#ifndef BYTECODE_VALUE_H
#define BYTECODE_VALUE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "method_table.h"
#include "debug.h"

struct bytecode_object;
struct activation_record;
struct bytecode_block;
typedef unsigned secure_t;
typedef struct value_t_struct {
    secure_t securelevel: 3;
    bool writability: 1;
    bool reference: 1;
    bool relocated: 1;
    bool block: 1;
    unsigned code_location: 32;
    union {
	int64_t i;
	struct bytecode_object *ref;
	struct activation_record *container;
    } contents;
} value_t;

extern __attribute__((gnu_inline)) inline value_t
value_new_integer(uint64_t i) {
    value_t res;
    res.contents.i = i;
    res.block = false;
    res.reference = false;
    return res;
}

extern __attribute__((gnu_inline)) inline value_t
value_new_block(location_t code_location,
		struct activation_record *rec) {
    value_t res;
    res.contents.container = (void*)rec;
    res.block = true;
    res.reference = false;
    res.code_location = code_location;
    return res;
}

extern __attribute__((gnu_inline)) inline value_t
value_new_reference(struct bytecode_object *ptr,
		    secure_t securelevel,
		    bool writability) {
    value_t res;
    res.reference = true;
    res.block = false;
    res.securelevel = securelevel;
    res.writability = writability;
    res.contents.ref = ptr;
    res.relocated = true;
    return res;
}

secure_t value_get_securelevel(value_t value);
value_t value_set_securelevel(value_t value, secure_t sl);
bool value_is_writable(value_t value);
value_t value_set_writable(value_t value, bool writable);
value_t value_make_trusted(value_t value);
#define value_is_reference(val) ((val).reference)
#define value_is_block(val) ((val).block)
#define value_get_integer(val) ((val).contents.i)
extern __attribute__((gnu_inline)) inline struct bytecode_object *
value_get_reference(value_t value) {
#if DEBUG
    if(!value.reference && !value.block) {
	fprintf(DEBUG_OUT,"smallintegers are not references\n");
	abort();
    }
#endif
    if(!value.relocated) {
	return relocate(value.contents.i);
    } else {
	return value.contents.ref;
    }
}

extern __attribute__((gnu_inline)) inline struct activation_record *
value_get_container(value_t val) {
    return val.contents.container;
}

extern __attribute__((gnu_inline)) inline void
value_relocate(value_t *value) {
    if(value->reference) {
	if(!value->relocated) {
	    value->contents.ref = relocate(value->contents.i);
	    value->relocated = true;
	}
    }
}

#if DEBUG >= 3
const char *value_to_string(value_t val);
#endif

#endif /* BYTECODE_VALUE_H */
