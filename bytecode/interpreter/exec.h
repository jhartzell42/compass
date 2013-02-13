#ifndef BYTECODE_EXEC_H
#define BYTECODE_EXEC_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "value.h"
#include "debug.h"

#define STACK_SIZE 12
#define TEMPORARY_SIZE 16
#define ARGS_SIZE 16

struct bytecode_block;
struct activation_record {
    struct bytecode_block *code; /* The code of this method/block. */
    value_t *args; /* This method's arguments. */
    int argc;
    value_t *block_args; /* This block's arguments. (NULL for methods) */
    int block_argc;
#if DEBUG
    int tempc;
#endif
    value_t self; /* Value method was called on. */
    void *operand_counter;
    value_t *temps; /* This method's temporary variables. */
    uint64_t sp; /* stack pointer */
    value_t stack[STACK_SIZE]; /* stack of this method/block */
    char *pc; /* Location in method; points to next instruction to exec.*/
    value_t res;
    struct activation_record *ret; /* Go here to return. 
				      NULL means entry point */
    struct activation_record *block_ret; /* Go here after evaling a block. 
					    NULL means method or
					    repeated block */
    struct activation_record *repeat_unwind; /* if block_ret is NULL,
						this is what the block
						repeat was invoked from*/
    struct activation_record *container; /* containing method.
					    NULL means method*/
    bool valid_return; /* true means it's OK to return from
			  this invocation */
    value_t catch_block;
#if DEBUG>=3
    int depth;
#endif
};

struct activation_record *
activation_record_new(struct activation_record *es, value_t target,
		      struct bytecode_block *meth, value_t *args,
		      int argc);

struct activation_record *
activation_record_block_new(struct activation_record *invoker,
			    struct activation_record *container,
			    struct bytecode_block *block,
			    value_t *blockargs, int argc);

void bytecode_run(struct activation_record *es);

value_t activation_record_pop(struct activation_record *restrict es);

void
activation_record_push(struct activation_record *restrict es, value_t val);

#define PEEK(rec) ((rec)->stack[(rec)->sp-1])
#if DEBUG
#define PUSH(rec,val) activation_record_push(rec,val)
#define POP(rec) activation_record_pop(rec)
#else
#define PUSH(rec,val) activation_record_push(rec,val)
#define POP(rec) activation_record_pop(rec)
#endif

#endif /* BYTECODE_EXEC_H */
