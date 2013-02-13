#ifndef BYTECODE_METHOD_H
#define BYTECODE_METHOD_H
#include "instruction.h"
#include "file.h"
#include "value.h"
#include "exec.h"

typedef enum { BYTECODE = 0, } builtin_t;
/* all other values mean builtin of some sort */

struct bytecode_block {
    builtin_t type;
    struct {
	location_t operands;
	char instructions[0];
    } bytecode;
};

value_t bytecode_block_evaluate(value_t self,
				int argc, value_t *argv,
				struct activation_record **es);

value_t bytecode_block_repeat(value_t self,
			      int argc, value_t *argv,
			      struct activation_record **es);

value_t bytecode_block_try(value_t self,
			   int argc, value_t *argv,
			   struct activation_record **es);

void do_bytecode_block_repeat(struct activation_record *container,
			      struct bytecode_block *code,
			      struct activation_record **rec);
value_t bytecode_evaluate_first_argument(value_t self,
					 int argc,
					 value_t *argv,
					 struct activation_record **rec);
value_t bytecode_evaluate_second_argument(value_t self,
					  int argc,
					  value_t *argv,
					  struct activation_record **rec);
#endif
