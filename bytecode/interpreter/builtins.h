#ifndef BYTECODE_BUILTINS_H
#define BYTECODE_BUILTINS_H
#include "value.h"
#include "instruction.h"

typedef value_t (*builtin_implementation_t)(value_t receiver,
					 int argc,
					 value_t *argv,
					 struct activation_record **rec);

value_t invoke_builtin(builtin_t which,
		    value_t receiver,
		    int argc,
		    value_t *argv,
		    struct activation_record **rec);

void register_builtin(builtin_t number, builtin_implementation_t builtin);
#endif
