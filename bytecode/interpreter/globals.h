#ifndef BYTECODE_GLOBALS_H
#define BYTECODE_GLOBALS_H
#include "value.h"

extern value_t bytecode_true;
extern value_t bytecode_false;
extern value_t bytecode_nil;

extern location_t smallinteger_mtable;
extern location_t block_object_mtable;

extern struct bytecode_block *initial_block;
#endif /* BYTECODE_GLOBALS_H */
