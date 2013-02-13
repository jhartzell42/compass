#ifndef BYTECODE_IO_H
#define BYTECODE_IO_H
#include "value.h"
#include "exec.h"

value_t bytecode_io_read_int(value_t receiver,
			     int argc,
			     value_t *argv,
			     struct activation_record **rec);

value_t bytecode_io_read_char(value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec);

value_t bytecode_io_print_int(value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec);

value_t bytecode_io_print_char(value_t receiver,
			       int argc,
			       value_t *argv,
			       struct activation_record **rec);

value_t bytecode_io_abort(value_t receiver,
			  int argc,
			  value_t *argv,
			  struct activation_record **rec);

value_t bytecode_io_print_method_table_name(value_t receiver,
					    int argc,
					    value_t *argv,
					    struct activation_record **rec);
value_t bytecode_io_stream_in(value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec);
value_t bytecode_io_stream_out(value_t receiver,
			       int argc,
			       value_t *argv,
			       struct activation_record **rec);
#endif /* BYTECODE_IO_H */
