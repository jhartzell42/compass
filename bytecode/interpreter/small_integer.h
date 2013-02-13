#ifndef BYTECODE_SMALL_INTEGER_H
#define BYTECODE_SMALL_INTEGER_H

#include "value.h"
#include "exec.h"

value_t bytecode_small_integer_add(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_small_integer_sub(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_small_integer_mul(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_small_integer_div(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_small_integer_mod(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_small_integer_negate(value_t receiver,
				      int argc,
				      value_t *argv,
				      struct activation_record **rec);
value_t bytecode_small_integer_less(value_t receiver,
				    int argc,
				    value_t *argv,
				    struct activation_record **rec);
value_t bytecode_small_integer_greater(value_t receiver,
				       int argc,
				       value_t *argv,
				       struct activation_record **rec);

#endif /* BYTECODE_SMALL_INTEGER_H */
