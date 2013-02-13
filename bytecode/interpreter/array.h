#ifndef BYTECODE_ARRAY_H
#define BYTECODE_ARRAY_H
value_t bytecode_reference_compare(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
value_t bytecode_store_instance_variable(value_t receiver,
					 int argc,
					 value_t *argv,
					 struct activation_record **rec);
value_t bytecode_get_instance_variable(value_t receiver,
				       int argc,
				       value_t *argv,
				       struct activation_record **rec);
value_t bytecode_byte_array_get(value_t receiver,
				int argc,
				value_t *argv,
				struct activation_record **rec);
value_t bytecode_byte_array_set(value_t receiver,
				int argc,
				value_t *argv,
				struct activation_record **rec);
value_t bytecode_byte_array_slice(value_t receiver,
				  int argc,
				  value_t *argv,
				  struct activation_record **rec);
value_t bytecode_byte_array_compare(value_t receiver,
				    int argc,
				    value_t *argv,
				    struct activation_record **rec);
value_t bytecode_byte_array_concat(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec);
#endif /* BYTECODE_ARRAY_H */
