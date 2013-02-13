/* All C code was placed in same file to allow for inter-procedural
   analysis to work better with GCC and for inlining to be more effective
*/
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gc/gc.h"

#include "block.h"
#include "builtins.h"
#include "debug.h"
#include "exec.h"
#include "file.h"
#include "globals.h"
#include "method_table.h"
#include "object.h"
#include "util.h"
#include "value.h"

static unsigned char *
get_char_array(value_t byte_array,int64_t *size);

static /*inline*/ void out_of_memory() {
    write(2,"out of memory\n",strlen("out of memory\n"));
    abort();
}

void *bytecode_malloc(size_t size) {
    void *res;

    if(size==0) {
	return NULL;
    }
    res = GC_MALLOC(size);

    if(res == NULL) {
	out_of_memory();
    }

    return res;
}

void *bytecode_malloc_no_pointers(size_t size) {
    void *res;
    if(size==0) {
	return NULL;
    }
    res = GC_MALLOC_ATOMIC(size);
    if(res == NULL) {
	out_of_memory();
    }
    return res;
}

/*inline*/ char *bytecode_strdup(const char *in) {
    char *res = bytecode_malloc_no_pointers(strlen(in)+1);
    strcpy(res,in);
    return res;
}

/*inline*/ void *bytecode_realloc(void *old, size_t size) {
    void *res;

    res = GC_REALLOC(old, size); 

    if(res == NULL) {
	out_of_memory();
    }

    return res;
}

char *bytecode_afgets(FILE *stream) {
    char *buf;
    int cur=0;
    int eof=1;
    buf = bytecode_malloc(80);
    while(1) {
	if(fgets(buf+cur,80,stream)==NULL)
	    break;
	eof = 0;
	if(buf[strlen(buf)-1]=='\n')
	    break;
	cur = strlen(buf);
	buf = bytecode_realloc(buf,cur+80);
    }
    return eof?NULL:buf;
}


/*inline*/ struct bytecode_object *object_new(size_t ivars) {
    return bytecode_malloc(sizeof(struct bytecode_object) +
			ivars * sizeof(value_t));
}

/*inline*/ location_t
object_get_table(struct bytecode_object *obj) {
    return obj->mtable;
}

/*inline*/ void object_set_table(struct bytecode_object *obj,
			     location_t tab) {
    obj->mtable = tab;
}

/*inline*/ value_t object_get_ivar(struct bytecode_object *obj, int num) {
    return obj->ivars[num];
}

/*inline*/ void object_set_ivar(struct bytecode_object *obj,
			    int num,
			    value_t val) {
    value_relocate(&val);
    obj->ivars[num] = val;
}

/*inline*/ value_t value_new_integer(uint64_t i) {
    value_t res;
    res.contents.i = i;
    res.reference = false;
    res.block = false;
    return res;
}

/*inline*/ value_t value_new_reference(struct bytecode_object *ptr,
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

/*inline*/ secure_t value_get_securelevel(value_t value) {
    return value.securelevel;
}

/*inline*/ value_t value_set_securelevel(value_t value, secure_t sl) {
    value_t res = value;
    res.securelevel = sl;
    return res;
}

/*inline*/ bool value_is_writable(value_t value) {
    return value.writability;
}

/*inline*/ value_t value_set_writable(value_t value, bool writable) {
    value_t res = value;
    res.writability = writable;
    return res;
}

/*inline*/ value_t value_make_trusted(value_t value) {
    value_t res = value;
    res.writability = true;
    res.securelevel = 0;
    return res;
}

/*inline*/ struct bytecode_object *value_get_reference(value_t value) {
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

/*inline*/ void value_relocate(value_t *value) {
    if(value->reference) {
	if(!value->relocated) {
	    value->contents.ref = relocate(value->contents.i);
	    value->relocated = true;
	}
    }
}

/*inline*/ value_t value_new_block(location_t code_location,
			struct activation_record *rec) {
    value_t res;
    res.contents.container = (void*)rec;
    res.block = true;
    res.reference = false;
    res.code_location = code_location;
    return res;
}

/*inline*/ struct activation_record *value_get_container(value_t val) {
    return val.contents.container;
}

#if DEBUG>=3
const char *value_to_string(value_t value) {
    value_relocate(&value);
    if(value.reference) {
	char *buf = bytecode_malloc_no_pointers(128);
	strcpy(buf,"Ref ");
	strcat(buf,try_translate_into_name(value.contents.ref));
	strcat(buf," ");
	const char *name = get_location_name(value.contents.ref->mtable);
	if(!strcmp("%AsciiString",name) ||
		!strcmp("%AsciiCharacter",name)) {
	    strcat(buf,"\"");
	    int64_t size;
	    char *v = get_char_array(value,&size);
	    *(buf+strlen(buf)+size) = 0;
	    strncpy(buf+strlen(buf), v, size);
	} else {
	    strcat(buf,name);
	}
	return buf;
    } else if(value.block) {
	char *buf = bytecode_malloc_no_pointers(128);
	strcpy(buf,"Block ");
	strcat(buf,try_translate_into_name(relocate(value.code_location)));
	return buf;
    } else {
	char *buf = bytecode_malloc_no_pointers(32);
	sprintf(buf,"Int %li",value_get_integer(value));
	return buf;
    }
}
#endif

int debug_printf(int depth, const char *fmt, ...) {
    int i;
    va_list ap;
    for(i=0; i<depth; i++) {
	printf("  ");
    }
    va_start(ap,fmt);
    vprintf(fmt,ap);
    va_end(ap);
}

/*inline*/ struct activation_record *
activation_record_new(struct activation_record *ar, value_t target,
		      struct bytecode_block *meth, value_t *args,
		      int argc) {
    struct activation_record *res =
	bytecode_malloc(sizeof(*res));
    res->ret = ar;

#if 0 /* assume all-bits-zero for NULL */
    /* used by bytecode_op_throw */
    res->block_ret = NULL; /* res->block_ret is blocks only */

    /* used by bytecode_op_block */
    res->container = NULL;
#endif

    res->self = target;
    res->code = meth;
    res->pc = meth->bytecode.instructions;
    res->operand_counter = relocate(meth->bytecode.operands);
    res->args = args;
    /* memcpy(res->args,args,argc*sizeof(value_t)); */
    res->argc = argc;
#if DEBUG
    res->block_args = NULL; /* res->block_args is blocks only */
#endif
#if DEBUG
    res->tempc = 0;
#endif
#if DEBUG>=3
    if(ar) {
	res->depth = ar->depth + 1;
    } else {
	res->depth = 0;
    }
#endif /* DEBUG>=3*/
#if PROFILE
    printf("CALLTO %s\n",try_translate_into_name(res->code));
#endif /* PROFILE */

    res->sp = 0;
    res->valid_return = true;
    /* res->catch_block = value_new_integer(0); -- guaranteed */

    return res;
}

/*inline*/ struct activation_record *
activation_record_block_new(struct activation_record *invoker,
			    struct activation_record *container,
			    struct bytecode_block *block,
			    value_t *block_args, int argc) {
    struct activation_record *res =
	bytecode_malloc(sizeof(*res));
    res->ret = container->ret;
    /* TODO: is this check redundant? */
    if(container->container != NULL) {
	res->container = container->container;
    } else {
	res->container = container;
    }
    res->valid_return = res->container->valid_return;
    res->block_ret = invoker;
    res->self = container->self;
    res->code = block;
    res->pc = block->bytecode.instructions;
    res->block_argc = argc;
    res->operand_counter = relocate(block->bytecode.operands);
#if PROFILE
    printf("CALLTO %s\n",try_translate_into_name(res->code));
#endif /*PROFILE*/
#if DEBUG>=3
    fprintf(DEBUG_OUT,"Operand pointer: %p\n",res->operand_counter);
    fprintf(DEBUG_OUT,"Operand file offset: %lx\n",
	    derelocate(res->operand_counter));
    fprintf(DEBUG_OUT,"Operands: %s\n",
	    try_translate_into_name(res->operand_counter));
#endif /*DEBUG>=3*/
    res->args = container->args;
    res->argc = container->argc;
#if DEBUG
    res->tempc = container->tempc;
#endif
#if DEBUG>=3
    if(invoker) {
	res->depth = invoker->depth + 1;
    } else {
	res->depth = 0;
    }
#endif
    res->block_args = block_args;

    res->temps = container->temps;

    res->catch_block = value_new_integer(0);
    res->sp = 0;
    return res;
}

/*inline*/ value_t activation_record_pop(struct activation_record *
				    restrict rec) {
#if DEBUG
    if(rec->sp == 0) {
	fprintf(DEBUG_OUT,"Stack underflow\n");
	abort();
    }
#endif
    value_t res = rec->stack[--rec->sp];
    DEBUG_PRINTF(rec->depth,"(Popping value: %s) SP=%lu\n",
		 value_to_string(res),
		 rec->sp);
    return res;
}

/*inline*/ void activation_record_push(struct activation_record *restrict rec,
			    value_t val) {
#if DEBUG
    if(rec->sp >= STACK_SIZE) {
	fprintf(DEBUG_OUT,"Stack overflow\n");
	abort();
    }
#endif
    rec->stack[rec->sp++] = val;
    DEBUG_PRINTF(rec->depth,"(Pushing value: %s) SP=%lu\n",
		 value_to_string(val),rec->sp);
}


/*inline*/ value_t bytecode_block_evaluate(value_t self,
				int argc, value_t *argv,
				struct activation_record **rec) {
    struct activation_record *container = value_get_container(self);
    struct bytecode_block *code = relocate(self.code_location);
#if DEBUG>=3
    printf("Entering block %s\n",try_translate_into_name(code));
#endif
    *rec = activation_record_block_new(*rec, container,
				       code, argv, argc);
    return value_new_integer(0);
}

/*inline*/ value_t bytecode_block_try(value_t self,
			   int argc, value_t *argv,
			   struct activation_record **rec) {
#if DEBUG>=3
    printf("Try ");
#endif
    bytecode_block_evaluate(self,0,NULL,rec);
    (*rec)->catch_block = argv[0];
    return value_new_integer(0);
}

/*inline*/ void do_bytecode_block_repeat(struct activation_record *container,
			      struct bytecode_block *code,
			      struct activation_record **rec) {
#if DEBUG>=3
    printf("Repeating block %s\n",try_translate_into_name(code));
#endif
    struct activation_record *old = *rec;
    *rec = activation_record_block_new(NULL, container,
				       code, NULL, 0);
    (*rec)->repeat_unwind = old;
}

/*inline*/ value_t bytecode_block_repeat(value_t self,
			      int argc, value_t *argv,
			      struct activation_record **rec) {
    struct activation_record *container = value_get_container(self);
    struct bytecode_block *code = relocate(self.code_location);
    do_bytecode_block_repeat(container,code,rec);
    return value_new_integer(0);
}


/*inline*/ value_t bytecode_evaluate_first_argument(value_t self,
					 int argc,
					 value_t *argv,
					 struct activation_record **rec) {
    return bytecode_block_evaluate(argv[0],0,NULL,rec);
}

/*inline*/ value_t bytecode_evaluate_second_argument(value_t self,
					  int argc,
					  value_t *argv,
					  struct activation_record **rec) {
    return bytecode_block_evaluate(argv[1],0,NULL,rec);
}
/*inline*/ value_t bytecode_reference_compare(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec) {
    value_t a = receiver;
    value_t b = argv[0];
    bool res;
    if(!value_is_reference(a) && !value_is_reference(b)) {
	res = value_get_integer(a) == value_get_integer(b);
    } else if(!value_is_reference(a) || !value_is_reference(b)) {
	res = false;
    } else if(value_is_block(a) || value_is_block(b)) {
	res = false;
    } else {
	res = value_get_reference(a) == value_get_reference(b);
    }
    return res?bytecode_true:bytecode_false;
}

static /*inline*/ int64_t primitive_array_get_size(value_t arr) {
    return value_get_integer(object_get_ivar(value_get_reference(arr),0));
}

/*inline*/ value_t bytecode_store_instance_variable(value_t receiver,
					 int argc,
					 value_t *argv,
					 struct activation_record **rec) {
    int64_t size = primitive_array_get_size(receiver);
    uint64_t index = value_get_integer(argv[0])+1;
    if(index-1>=size) {
	return bytecode_block_evaluate(argv[2],0,NULL,rec);
    }
    object_set_ivar(value_get_reference(receiver),index,argv[1]);
    return receiver;
}

/*inline*/ value_t bytecode_get_instance_variable(value_t receiver,
				       int argc,
				       value_t *argv,
				       struct activation_record **rec) {
    int64_t size = primitive_array_get_size(receiver);
    uint64_t index = value_get_integer(argv[0])+1;
    if(index-1>=size) {
	return bytecode_block_evaluate(argv[1],0,NULL,rec);
    }
    return object_get_ivar(value_get_reference(receiver),index);
}

static unsigned char *
get_char_array(value_t byte_array,int64_t *size) {
    struct bytecode_object *obj = value_get_reference(byte_array);
    if(size!=NULL) {
	*size = value_get_integer(object_get_ivar(obj,0));
    }
    value_t byte_array_val = object_get_ivar(obj,1);
    return (unsigned char*)value_get_reference(byte_array_val);
}

/*inline*/ value_t bytecode_byte_array_get(value_t receiver,
				int argc,
				value_t *argv,
				struct activation_record **rec) {
    uint64_t index = value_get_integer(argv[0]);
    uint64_t size = primitive_array_get_size(receiver);
    if(index>=size) {
	return bytecode_block_evaluate(argv[1],0,NULL,rec);
    }
    struct bytecode_object *arr = value_get_reference(receiver);
    value_t byte_array_val = object_get_ivar(arr,1);
    unsigned char *byte_array = (char*)value_get_reference(byte_array_val);
    return value_new_integer(byte_array[index]);
}

/*inline*/ value_t bytecode_byte_array_set(value_t receiver,
				int argc,
				value_t *argv,
				struct activation_record **rec) {
    uint64_t index = value_get_integer(argv[0]);
    int64_t to_assign = value_get_integer(argv[1]);
    struct bytecode_object *arr = value_get_reference(receiver);
    uint64_t size = primitive_array_get_size(receiver);
    if(index>=size) {
	return bytecode_block_evaluate(argv[2],0,NULL,rec);
    }
    value_t byte_array_val = object_get_ivar(arr,1);
    unsigned char *byte_array = (char*)value_get_reference(byte_array_val);
    byte_array[index] = (unsigned char)to_assign;
    return receiver;
}

/*inline*/ static value_t bytecode_array_single_slice(value_t receiver,
				    int argc,
				    value_t *argv,
				    struct activation_record **rec) {
    uint64_t start = value_get_integer(argv[0]);
    uint64_t finish = value_get_integer(argv[1]);
    uint64_t size;
    unsigned char *byte_array = get_char_array(receiver,&size);
    if(finish>=size) {
	return bytecode_block_evaluate(argv[3],0,NULL,rec);
    }
    int64_t new_size = finish-start+1;
    if(start>finish) {
	new_size = 0;
    }
    unsigned char *new_array = byte_array + start;
    struct bytecode_object *res = object_new(2);
    object_set_ivar(res,0,value_new_integer(new_size));
    object_set_ivar(res,1,value_new_reference((void*)new_array,0,0));
    argv = 0;
    return value_new_reference(res,0,0);
}

#define READ_SIZE 1024

/*inline*/ static value_t byte_array_slurp(value_t receiver,
				       int argc,
				       value_t *argv,
				       struct activation_record **rec) {
    int fd = value_get_integer(	
		object_get_ivar(value_get_reference(receiver),0));
    uint64_t length = READ_SIZE;
    uint64_t remaining = length;
    char *data = bytecode_malloc_no_pointers(length);
    int64_t cur_pos = 0;
    int read_count;
    while(read_count = read(fd,data + cur_pos,remaining)) {
	cur_pos += read_count;
	remaining -= read_count;
	if(remaining == 0) {
	    length += READ_SIZE;
	    remaining += READ_SIZE;
	    data = bytecode_realloc(data,length);
	}
    }
    length -= remaining;
    value_t arr = value_new_reference((void*)data,0,0);
    struct bytecode_object *str_obj = object_new(2);
    object_set_ivar(str_obj,0,value_new_integer(length));
    object_set_ivar(str_obj,1,arr);
    return value_new_reference(str_obj,0,0);
}

/*inline*/ value_t bytecode_byte_array_concat(value_t receiver,
				   int argc,
				   value_t *argv,
				   struct activation_record **rec) {
    int64_t size1, size2;
    unsigned char *byte_array1 = get_char_array(receiver,&size1);
    unsigned char *byte_array2 = get_char_array(argv[0],&size2);
    int64_t res_size = size1 + size2;
    unsigned char *res_array = bytecode_malloc_no_pointers(res_size);
    if(size1 > 0) {
	memcpy(res_array, byte_array1, size1);
    }
    if(size2 > 0) {
	memcpy(res_array + size1, byte_array2, size2);
    }
    struct bytecode_object *res = object_new(2);
    object_set_ivar(res,0,value_new_integer(res_size));
    object_set_ivar(res,1,value_new_reference((void*)res_array,0,0));
    return value_new_reference(res,0,0);
}

/*inline*/ value_t bytecode_byte_array_slice(value_t receiver,
				  int argc,
				  value_t *argv,
				  struct activation_record **rec) {
    int64_t start = value_get_integer(argv[0]);
    int64_t step = value_get_integer(argv[2]);
    if(step == 1) {
	return bytecode_array_single_slice(receiver,
					   argc,
					   argv,
					   rec);
    }
    int64_t finish = (value_get_integer(argv[1])-start)/step*step+start;
    int64_t size;
    unsigned char *byte_array = get_char_array(receiver,&size);
    int64_t new_size = ((finish-start)/step)+1;
    if(start>finish) {
	new_size = 0;
    }
    unsigned char *new_byte_array = bytecode_malloc_no_pointers(new_size);
    uint64_t old_index = start;
    int64_t i;
    for(i = 0;
	    i < new_size && old_index < size;
	    i++,old_index+=step) {
	new_byte_array[i] = byte_array[old_index];
    }
    new_size = i;
    struct bytecode_object *res = object_new(2);
    object_set_ivar(res,0,value_new_integer(new_size));
    object_set_ivar(res,1,value_new_reference((void*)new_byte_array,0,0));
    return value_new_reference(res,0,0);
}

/* compare receiver against argv[0] and if:
 * receiver is less,	    return argv[1]
 * receiver is greater,	    return argv[2]
 * they are equal,	    return argv[3]
 */
/*inline*/ value_t bytecode_byte_array_compare(value_t receiver,
				    int argc,
				    value_t *argv,
				    struct activation_record **rec) {
    int64_t size1;
    int64_t size2;
    unsigned char *arr1 = get_char_array(receiver,&size1);
    unsigned char *arr2 = get_char_array(argv[0],&size2);
    int64_t smaller_size = size1<size2?
			    size1:
			    size2;
    int cmp;
    if(smaller_size) {
	cmp = strncmp(arr1,arr2,smaller_size);
    } else {
	cmp = 0;
    }
    if(cmp<0) {
	return argv[1];
    } else if(cmp==0) {
	if(size1<size2) {
	    return argv[1];
	} else if(size1>size2) {
	    return argv[2];
	} else {
	    return argv[3];
	}
    } else if(cmp>0) {
	return argv[2];
    }
}

/*inline*/ value_t bytecode_small_integer_add(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    return value_new_integer(self + other);
}
/*inline*/ value_t bytecode_small_integer_sub(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    return value_new_integer(self - other);
}
/*inline*/ value_t bytecode_small_integer_mul(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    return value_new_integer(self * other);
}
/*inline*/ value_t bytecode_small_integer_div(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    if(other == 0) {
	/*TODO: throw exception*/
	fprintf(stderr,"Divide by zero\n");
	abort();
    }
    return value_new_integer(self / other);
}
/*inline*/ value_t bytecode_small_integer_mod(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    if(other == 0) {
	/*TODO: throw exception*/
	fprintf(stderr,"Divide by zero\n");
	abort();
    }
    return value_new_integer(self % other);
}
/*inline*/ value_t bytecode_small_integer_negate(value_t receiver,
				      int argc, value_t *argv,
				      struct activation_record **rec) {
    int64_t self;
    self = value_get_integer(receiver);
    return value_new_integer(-self);
}
/*inline*/ value_t bytecode_small_integer_less(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    return (self < other)?bytecode_true:bytecode_false;
}
/*inline*/ value_t bytecode_small_integer_greater(value_t receiver,
				   int argc, value_t *argv,
				   struct activation_record **rec) {
    int64_t self, other;
    self = value_get_integer(receiver);
    other = value_get_integer(argv[0]);
    return (self > other)?bytecode_true:bytecode_false;
}

/*inline*/ value_t bytecode_io_stream_out(value_t receiver,
			       int argc,
			       value_t *argv,
			       struct activation_record **rec) {
    int fd =
	value_get_integer(value_get_reference(receiver)->ivars[0]);
    char to_write = value_get_integer(argv[0]);
    write(fd,&to_write,1);
    return receiver;
}

/*inline*/ value_t bytecode_io_stream_in(value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec) {
    int fd =
	value_get_integer(value_get_reference(receiver)->ivars[0]);
    char to_read;
    if(read(fd,&to_read,1)==1)
	return value_new_integer(to_read);
    else
	return value_new_integer(-1);
}

/*inline*/ value_t bytecode_io_abort(value_t receiver,
			  int argc,
			  value_t *argv,
			  struct activation_record **rec) {
    fprintf(DEBUG_OUT,"Abort method invoked.\n");
    abort();
}

/*inline*/ value_t bytecode_io_print_method_table_name(value_t receiver,
					    int argc,
					    value_t *argv,
					    struct activation_record **rec) {
    if(value_is_reference(receiver)) {
	location_t mtable = value_get_reference(receiver)->mtable;
	printf("%s\n",get_location_name(mtable));
    } else if(value_is_block(receiver)) {
	printf("%%BlockObject\n");
    } else {
	printf("%%SmallInteger\n");
    }
    return receiver;
}
static long bytecount;


/*inline*/ value_t bytecode_return_true(value_t receiver,
			     int argc,
			     value_t *argv,
			     struct activation_record **rec) {
    return bytecode_true;
}

/*inline*/ value_t bytecode_return_false(value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec) {
    return bytecode_false;
}

/*inline*/ value_t invoke_builtin(builtin_t which,
			      value_t receiver,
			      int argc,
			      value_t *argv,
			      struct activation_record **rec) {
    switch(which) {
	case 1:
	    return bytecode_small_integer_add(receiver,argc,argv,rec);
	case 2:
	    return bytecode_small_integer_sub(receiver,argc,argv,rec);
	case 3:
	    return bytecode_small_integer_mul(receiver,argc,argv,rec);
	case 4:
	    return bytecode_small_integer_div(receiver,argc,argv,rec);
	case 5:
	    return bytecode_small_integer_mod(receiver,argc,argv,rec);
	case 6:
	    return bytecode_small_integer_negate(receiver,argc,argv,rec);
	case 7:
	    return bytecode_small_integer_less(receiver,argc,argv,rec);
	case 8:
	    return bytecode_small_integer_greater(receiver,argc,argv,rec);
	case 9:
	    return bytecode_block_evaluate(receiver,argc,argv,rec);
	case 14:
	    return bytecode_io_abort(receiver,argc,argv,rec);
	case 15:
	    return bytecode_io_print_method_table_name(receiver,
						       argc,
						       argv,
						       rec);
	case 16:
	    return bytecode_block_repeat(receiver,argc,argv,rec);
	case 17:
	    return bytecode_block_try(receiver,argc,argv,rec);
	case 18:
	    return bytecode_io_stream_out(receiver,argc,argv,rec);
	case 19:
	    return bytecode_io_stream_in(receiver,argc,argv,rec);
	case 20:
	    return bytecode_reference_compare(receiver,argc,argv,rec);
	case 21:
	    return bytecode_store_instance_variable(receiver,
						    argc,
						    argv,
						    rec);
	case 22:
	    return bytecode_get_instance_variable(receiver,argc,argv,rec);
	case 23:
	    return bytecode_byte_array_get(receiver,argc,argv,rec);
	case 24:
	    return bytecode_byte_array_set(receiver,argc,argv,rec);
	case 25:
	    return bytecode_byte_array_slice(receiver,argc,argv,rec);
	case 26:
	    return bytecode_byte_array_compare(receiver,argc,argv,rec);
	case 27:
	    return bytecode_evaluate_first_argument(receiver,
						    argc,
						    argv,
						    rec);
	case 28:
	    return bytecode_evaluate_second_argument(receiver,
						     argc,
						     argv,
						     rec);
	case 29:
	    return bytecode_return_true(receiver,argc,argv,rec);
	case 30:
	    return bytecode_return_false(receiver,argc,argv,rec);
	case 31:
	    return bytecode_byte_array_concat(receiver,argc,argv,rec);
	case 32:
	    return byte_array_slurp(receiver,argc,argv,rec);
	default:
	    fprintf(stderr,"Bad builtin #%lu\n",which);
	    abort();
    }
}

static /*inline*/ uint64_t
next_integer_operand(struct activation_record *rec) {
    uint64_t *operand_counter = rec->operand_counter;
    uint64_t res = *operand_counter++;
    rec->operand_counter = operand_counter;
    return res;
}

static /*inline*/ struct activation_record *
bytecode_do_call(int argc, struct bytecode_block *meth,
		 struct activation_record *curr,
		 value_t target);

static /*inline*/ struct activation_record *
bytecode_op_throw(struct activation_record *curr);
static /*inline*/ struct activation_record *
bytecode_op_true(struct activation_record *curr);

static /*inline*/ struct activation_record *
bytecode_op_false(struct activation_record *curr);

static /*inline*/ void
bytecode_boolean(bool b, struct activation_record *curr) {
    value_t val = b?bytecode_true:bytecode_false;
    DEBUG_PRINTF(curr->depth,b?"->True\n":"->False\n");
    PUSH(curr,val);
}

static /*inline*/ value_t bytecode_alloc_object(uint64_t ivars) {
    return value_new_reference(object_new(ivars), 0, true);
}

static /*inline*/ struct activation_record *
bytecode_op_alloc(struct activation_record *curr) {
    uint64_t ivar_count = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Alloc %li\n",ivar_count);
    PUSH(curr, bytecode_alloc_object(ivar_count));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_alloc_ind(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"AllocInd\n");
    PUSH(curr, bytecode_alloc_object(
	    value_get_integer(POP(curr))));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_alloc_byte_array_ind(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"AllocByteArrayInd\n");
    uint64_t size = value_get_integer(POP(curr));
    value_t res = bytecode_alloc_object(2);
    value_t arr = value_new_reference(bytecode_malloc_no_pointers(size),
				      0,0);
    object_set_ivar(value_get_reference(res),0,value_new_integer(size));
    object_set_ivar(value_get_reference(res),1,arr); 
    PUSH(curr,res);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_into_byte_array(struct activation_record *curr) {
    uint64_t index = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreIntoByteArray %lu\n",index);
    struct bytecode_object *arr = value_get_reference(POP(curr));
    uint64_t int_to_store = value_get_integer(POP(curr));
    value_t byte_array_val = object_get_ivar(arr,1);
    char *byte_array = (unsigned char*)value_get_reference(byte_array_val);
    byte_array[index] = (unsigned char)int_to_store;
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_index_byte_array_ind(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"IndexByteArrayInd\n");
    uint64_t index = value_get_integer(POP(curr));
    struct bytecode_object *arr = value_get_reference(POP(curr));
    value_t byte_array_val = object_get_ivar(arr,1);
    char *byte_array = (unsigned char*)value_get_reference(byte_array_val);
    PUSH(curr,value_new_integer(byte_array[index]));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_arg(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Arg %li\n",which);
    PUSH(curr, curr->args[which]);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_arg(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreArg %li\n",which);
    curr->args[which] = POP(curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_block(struct activation_record *curr) {
    location_t arg = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Block %s\n",get_location_name(arg));
    struct activation_record *container;
    if(curr->container != NULL) {
	container = curr->container;
    } else {
	container = curr;
    }
    PUSH(curr,value_new_block(arg,container));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_block_arg(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"BlockArg %li\n",which);
    if(which >= curr->block_argc) {
	PUSH(curr, value_new_integer(-1));
	return bytecode_op_throw(curr);
    }
    PUSH(curr, curr->block_args[which]);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_block_arg(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreBlockArg %li\n",which);
    if(which >= curr->block_argc) {
	// XXX actually "throw exception" here
	abort();
    }
    curr->block_args[which] = POP(curr);
    return curr;
}

static /*inline*/ void
set_mtable(value_t val, location_t mtable) {
    struct bytecode_object *obj = value_get_reference(val);
    object_set_table(obj,mtable);
}

static /*inline*/ struct activation_record *
bytecode_op_call(struct activation_record *curr) {
    uint64_t argc = next_integer_operand(curr);
    location_t block = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Call %lu %s\n",
		 argc,
		 get_location_name(block));
    value_t target = POP(curr);
    return bytecode_do_call(argc,relocate(block),curr,target);
}

static /*inline*/ struct activation_record *
bytecode_op_ch_m_table(struct activation_record *curr) {
    location_t mtable = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"ChMTable %s\n",get_location_name(mtable));
    set_mtable(POP(curr),mtable);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_dup(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Dup\n");
    PUSH(curr, PEEK(curr));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_equals(struct activation_record *curr) {
    value_t a, b;
    DEBUG_PRINTF(curr->depth,"Equals\n");
    a = POP(curr);
    b = POP(curr);
    /*DEBUG_PRINTF(curr->depth,"A=%s\n",value_to_string(a));
      DEBUG_PRINTF(curr->depth,"B=%s\n",value_to_string(b));
      DEBUG_PRINTF(curr->depth,a.block?"A is block\n":"A is not block\n");
      DEBUG_PRINTF(curr->depth,b.block?"B is block\n":"B is not block\n");*/
    if(value_is_reference(a) != value_is_reference(b)) {
	DEBUG_PRINTF(curr->depth,"reference bits don't match\n");
	bytecode_boolean(false, curr);
	return curr;
    } else if(value_is_block(a) || value_is_block(b)) {
	DEBUG_PRINTF(curr->depth,"blocks are being compared\n");
	bytecode_boolean(false,curr);
	return curr;
    } else if(!value_is_reference(a)) {
	DEBUG_PRINTF(curr->depth,"Integers are being compared\n");
	bytecode_boolean(value_get_integer(a) == value_get_integer(b),
			 curr);
	return curr;
    } else {
	DEBUG_PRINTF(curr->depth,"References are being compared\n");
	bytecode_boolean(value_get_reference(a) == value_get_reference(b),
			 curr);
	return curr;
    }
}

static /*inline*/ struct activation_record *
bytecode_op_false(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"False\n");
    bytecode_boolean(false,curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_finish(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"BlockReturn\n");
    if(curr->block_ret != NULL) {
	struct activation_record *ret = curr->block_ret;
	DEBUG_PRINTF(0,"Returning into block: %s\n",
		     try_translate_into_name(ret->code));
	PUSH(ret, POP(curr));
	return ret;
    } else {
	struct activation_record *ret = curr->repeat_unwind;
	do_bytecode_block_repeat(curr->container,
				 curr->code,
				 &ret);
	return ret;
    }
}

static /*inline*/ struct activation_record *
bytecode_op_global(struct activation_record *curr) {
    location_t glb_loc = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Global %s\n",get_location_name(glb_loc));
    value_t *glb = relocate(glb_loc);
    PUSH(curr, *glb);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_inst(struct activation_record *curr) {
    struct bytecode_object *obj;
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Inst %li\n",which);
    obj = value_get_reference(POP(curr));
    PUSH(curr, object_get_ivar(obj, which));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_inst_ind(struct activation_record *curr) {
    struct bytecode_object *obj;
    int n;
    DEBUG_PRINTF(curr->depth,"InstInd\n");
    obj = value_get_reference(POP(curr));
    n = value_get_integer(POP(curr));
    PUSH(curr, object_get_ivar(obj, n));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_int(struct activation_record *curr) {
    uint64_t integer = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Int %li\n",integer);
    PUSH(curr, value_new_integer(integer));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_is_rw(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"IsRw\n");
    value_t val = POP(curr);
    bool res = value_is_writable(val);
    bytecode_boolean(res,curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_do_call(int argc, struct bytecode_block *meth,
		 struct activation_record *curr,
		 value_t target) {
    int i;
#if DEBUG
    if(meth == NULL) {
	fprintf(DEBUG_OUT, "bytecode_do_call() on invalid method.\n");
	abort();
    }
#endif
    value_t *args = bytecode_malloc(sizeof(value_t) * argc);
    curr->sp -= argc;
    memcpy(args,curr->stack + curr->sp,argc*sizeof(value_t));
#if DEBUG >= 3
    fprintf(DEBUG_OUT,"Entering block %s\n", try_translate_into_name(meth));
#endif
    if(meth->type == BYTECODE) {
	struct activation_record *nar;
	nar = activation_record_new(curr, target, meth, args, argc);
	return nar;
    } else {
	DEBUG_PRINTF(0,"(bytecode builtin method)\n");
	value_t res = invoke_builtin(meth->type,target,argc,args,&curr);
	PUSH(curr,res);
	return curr;
    }
}

static /*inline*/ struct bytecode_block *
method_table_get(struct bytecode_method_table *restrict tab,
		 selector_t sel) {
    int size_mask = tab->size;
    int i = sel&size_mask;
    struct method_table_entry *entries = tab->entries;
    struct bytecode_block *res = relocate(entries[i].block_offset);
    if(entries[i].sel == sel) {
	return res;
    }
    for(i = (sel+1)&size_mask;
	entries[i].block_offset != 0;
	i = (i + 1)&size_mask) {
	if(entries[i].sel == sel) {
	    return relocate(entries[i].block_offset);
	}
    }
    DEBUG_PRINTF(0,"Message not found in: %s\n",
		 try_translate_into_name(tab));
    return relocate(tab->default_block_offset);
}

static /*inline*/ struct activation_record *
bytecode_do_m_call(struct activation_record *curr,
		   value_t target) {
    struct bytecode_block *meth;
    uint64_t argc = next_integer_operand(curr);
    selector_t sel = next_integer_operand(curr);
    DEBUG_PRINTF(0,"MCall %li %li \n",argc,sel);
    if(value_is_reference(target)) {
	meth =
	    method_table_get(relocate(value_get_reference(target)->mtable),
			     sel);
    } else if(value_is_block(target)) {
	meth = method_table_get(relocate(block_object_mtable),sel);
    } else  {
	meth = method_table_get(relocate(smallinteger_mtable),sel);
    }
#if DEBUG
    if(meth == NULL) {
	abort();
    }
#endif

    return bytecode_do_call(argc, meth, curr, target);
}

static /*inline*/ struct activation_record *
bytecode_op_m_call(struct activation_record *curr) {
    value_t target = POP(curr);
    DEBUG_PRINTF(curr->depth,"");
    return bytecode_do_m_call(curr, target);
}

static /*inline*/ struct activation_record *
bytecode_op_block_evaluate(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"BlockEvaluate: ");
    value_t target = POP(curr);
    if(value_is_block(target)) {
	uint64_t argc = next_integer_operand(curr);
	next_integer_operand(curr);
	DEBUG_PRINTF(0,"%li ignore\n",argc);
	value_t *argv = bytecode_malloc(sizeof(value_t) * argc);
	int i;
	for(i = argc-1; i>=0; i++) {
	    argv[i] = POP(curr);
	}
	struct activation_record *res = curr;
	bytecode_block_evaluate(target,argc,argv,&res);
	return res;
    } else {
	bytecode_do_m_call(curr,target);
	return curr;
    }
}

static /*inline*/ struct activation_record *
bytecode_op_nil(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Nil\n");
    PUSH(curr,bytecode_nil);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_pop(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Pop\n");
    POP(curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_ref_equals(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"RefEquals\n");
    value_t a, b;
    a = POP(curr);
    b = POP(curr);
    if(!value_is_reference(a) && !value_is_reference(b)) {
	bytecode_boolean(value_get_integer(a) == value_get_integer(b),
			 curr);
    } else if(!value_is_reference(a) || !value_is_reference(b)) {
	bytecode_boolean(false, curr);
    } else if(value_is_block(a) || value_is_block(b)) {
	bytecode_boolean(false,curr);
    } else {
	bytecode_boolean(value_get_reference(a) == value_get_reference(b),
			 curr);
    }
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_ro(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Ro\n");
    value_t val;
    val = POP(curr);
    PUSH(curr,value_set_writable(val,false));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_rw(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Rw\n");
    value_t val;
    val = POP(curr);
    PUSH(curr,value_set_writable(val,true));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_ch_m_table(struct activation_record *curr) {
    location_t mtable = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"SChMTable %s\n",get_location_name(mtable));
    set_mtable(curr->self,mtable);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_secure(struct activation_record *curr) {
    value_t obj,sl_val,secured_obj;
    secure_t sl = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Secure %i\n",sl);
    obj = POP(curr);
    sl_val = POP(curr);
    sl = value_get_securelevel(sl_val);
    secured_obj = value_set_securelevel(obj,sl);
    PUSH(curr,secured_obj);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_do_return(struct activation_record *curr, value_t ret) {
    if(curr->container) {
	struct activation_record *container = curr->container;
	if(curr->valid_return && container->valid_return) {
	    container->valid_return = false;
	} else {
	    fprintf(DEBUG_OUT,"Multiple returns attempted\n");
	    /* TODO - throw "exception" - this is so something
	     * a compass program could do */
	    abort();
	}
    }
    curr->valid_return = false;
    if(curr->ret == NULL) {
	/* Returning from initial code. */
#if COUNTER
	fprintf(stderr,"Total bytes executed: %lu\n",bytecount);
#endif
	exit(0);
    } else {
	curr = curr->ret;
    }
    DEBUG_PRINTF(0,"Returning into block: %s\n",
		 try_translate_into_name(curr->code));
#if DEBUG>=3
    if(curr->container != NULL) {
	DEBUG_PRINTF(0,"Context block: %s\n",
		     try_translate_into_name(curr->container->code));
    }
#endif
    DEBUG_PRINTF(0,"With %i temporaries\n",curr->tempc);
    PUSH(curr, ret);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_return(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Return\n");
    return bytecode_do_return(curr, POP(curr));
}

static /*inline*/ struct activation_record *
bytecode_op_self(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Self\n");
    PUSH(curr, curr->self);
    return curr;
}

/* TODO: do we really need this byte? I mean, seriously...*/
static /*inline*/ struct activation_record *
bytecode_op_swap(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Swap\n");
    value_t a, b;
    a = POP(curr);
    b = POP(curr);
    PUSH(curr, a);
    PUSH(curr, b);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_inst(struct activation_record *curr) {
    struct bytecode_object *self;
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"SInst %i\n",which);
    self = value_get_reference(curr->self);
    PUSH(curr, object_get_ivar(self, which));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_inst_ind(struct activation_record *curr) {
    struct bytecode_object *self;
    int n;
    DEBUG_PRINTF(curr->depth,"SInstInd\n");
    self = value_get_reference(curr->self);
    n = value_get_integer(POP(curr));
    PUSH(curr, object_get_ivar(self, n));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_is_rw(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"IsRw\n");
    bool res = value_is_writable(curr->self);
    bytecode_boolean(res,curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_sl(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Sl\n");
    value_t obj;
    obj = POP(curr);
    PUSH(curr,
	 value_new_integer(value_get_securelevel(obj)));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_m_call(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"S");
    return bytecode_do_m_call(curr, curr->self);
}

static /*inline*/ struct activation_record *
bytecode_op_s_return(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"SReturn\n");
    return bytecode_do_return(curr, curr->self);
}

static /*inline*/ struct activation_record *
bytecode_op_s_ro(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"SRo\n");
    value_t self;
    self = curr->self;
    PUSH(curr,value_set_writable(self,false));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_rw(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"SRw\n");
    value_t self;
    self = curr->self;
    PUSH(curr,value_set_writable(self,true));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_store_inst(struct activation_record *curr) {
    struct bytecode_object *self;
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"SStoreInst %li\n",which);
    value_t v;
    self = value_get_reference(curr->self);
    v = POP(curr);
    object_set_ivar(self, which, v);
    return curr;
}

static /*inline*/ struct activation_record *

bytecode_op_s_store_inst_ind(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"SStoreInstInd\n");
    struct bytecode_object *self;
    value_t v;
    int n;
    self = value_get_reference(curr->self);
    n = value_get_integer(POP(curr));
    v = POP(curr);
    object_set_ivar(self, n, v);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_secure(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"SSecure\n");
    value_t sl_val,secured_obj;
    secure_t sl;
    sl_val = POP(curr);
    sl = value_get_securelevel(sl_val);
    secured_obj = value_set_securelevel(curr->self,sl);
    PUSH(curr,secured_obj);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_s_sl(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Sl\n");
    PUSH(curr, value_new_integer(
	    value_get_securelevel(curr->self)));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_global(struct activation_record *curr) {
    location_t glb_loc = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreGlobal %s\n",get_location_name(glb_loc));
    value_t *glb = relocate(glb_loc);
    *glb = POP(curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_inst(struct activation_record *curr) {
    struct bytecode_object *obj;
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreInst %li\n",which);
    value_t v;
    obj = value_get_reference(POP(curr));
    v = POP(curr);
    object_set_ivar(obj, which, v);
    return curr;
}

static /*inline*/ struct activation_record *

bytecode_op_store_inst_ind(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"StoreInstInd\n");
    struct bytecode_object *obj;
    int n;
    value_t v;
    obj = value_get_reference(POP(curr));
    n = value_get_integer(POP(curr));
    v = POP(curr);
    object_set_ivar(obj, n, v);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_store_temp(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"StoreTemp %li\n",which);
#if DEBUG
    if(which >= curr->tempc) {
	fprintf(DEBUG_OUT,"non-existent temporary %li\n",which);
	abort();
    }
#endif
    curr->temps[which] = POP(curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_temp(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Temp %li\n",which);
    PUSH(curr, curr->temps[which]);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_temporaries(struct activation_record *curr) {
    uint64_t which = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"Temporaries %li\n",which);
    curr->temps = bytecode_malloc(sizeof(value_t) * which);
#if DEBUG
    curr->tempc = which;
#endif
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_true(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"True\n");
    bytecode_boolean(true,curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_t_self(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"TSelf\n");
    PUSH(curr, value_make_trusted(curr->self));
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_t_s_m_call(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"TS");
    return bytecode_do_m_call(curr, value_make_trusted(curr->self));
}

static /*inline*/ struct activation_record *
bytecode_op_alloc_from_args(struct activation_record *curr) {
    int argc = curr->argc;
    struct bytecode_object *obj = object_new(argc+1);
    int i;
    DEBUG_PRINTF(curr->depth,"AllocFromArgs\n");
    obj->ivars[0] = value_new_integer(argc);
    for(i=0; i<argc; i++) {
	obj->ivars[i+1] = curr->args[i];
    }
    PUSH(curr,value_new_reference(obj,0,false));
    return curr;
}

static /*inline*/ struct activation_record *
find_nearest_catch(struct activation_record *curr) {
    if(curr == NULL) {
	DEBUG_PRINTF(0,"Unrolled to program init: aborting\n");
	return curr;
    }
    DEBUG_PRINTF(0,"Invocation of %s\n",
		 try_translate_into_name(curr->code));
    DEBUG_PRINTF(0,"Value of self: %s\n",
		 value_to_string(curr->self));
    DEBUG_PRINTF(0,"\n");
    if(curr == NULL || value_is_block(curr->catch_block)) {
	return curr;
    } else if(curr->block_ret != NULL) {
	return find_nearest_catch(curr->block_ret);
    } else if(curr->container != NULL) {
	DEBUG_PRINTF(0,"Invocation of #BlockRepeat\n");
	return find_nearest_catch(curr->repeat_unwind);
    } else {
	return find_nearest_catch(curr->ret);
    }
}

static /*inline*/ struct activation_record *
bytecode_op_throw(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Throw\n");
    value_t to_throw = POP(curr);
    DEBUG_PRINTF(0,"Traceback:\n");
    curr = find_nearest_catch(curr);
    if(curr == NULL) {
	bytecode_io_print_method_table_name(to_throw,0,NULL,NULL);
	bytecode_io_abort(to_throw,0,NULL,NULL);
    }
    value_t catch_block = curr->catch_block;
    curr = curr->block_ret;
    value_t *argv = bytecode_malloc(sizeof(value_t));
    argv[0] = to_throw;
    bytecode_block_evaluate(catch_block,1,argv,&curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_block_args(struct activation_record *curr) {
    uint64_t begin = next_integer_operand(curr);    
    uint64_t end = next_integer_operand(curr);
    DEBUG_PRINTF(curr->depth,"BlockArgs %i %i\n",begin,end);
    uint64_t copy_count = end-begin+1;
    if(copy_count > curr->block_argc) {
	DEBUG_PRINTF(curr->depth,"(too few arguments)\n");
	PUSH(curr,value_new_integer(-1));
	return bytecode_op_throw(curr);
    }
    uint64_t i;
    memcpy(curr->temps + begin, curr->block_args,
	   sizeof(value_t) * copy_count);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_add(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Add: \n");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t res = value_get_integer(target)+value_get_integer(arg);
	PUSH(curr,value_new_integer(res));
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_subtract(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Subtract: \n");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t res = value_get_integer(target)-value_get_integer(arg);
	PUSH(curr,value_new_integer(res));
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_multiply(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Multiply: \n");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(curr->depth,"Numbers\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t res = value_get_integer(target)*value_get_integer(arg);
	PUSH(curr,value_new_integer(res));
	return curr;
    } else {
	DEBUG_PRINTF(curr->depth,"MCalling:\n");
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_divide(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Divide: \n");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t res = value_get_integer(target)/value_get_integer(arg);
	PUSH(curr,value_new_integer(res));
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_mod(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Mod: ");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t res = value_get_integer(target)%value_get_integer(arg);
	PUSH(curr,value_new_integer(res));
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_greater(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Greater: ");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	bytecode_boolean(value_get_integer(target)>value_get_integer(arg),
			 curr);
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_integer_equals(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"IntegerEquals: ");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	bytecode_boolean(value_get_integer(target)==value_get_integer(arg),
			 curr);
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_less(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Less: ");
    value_t target = POP(curr);
    value_t arg = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg) &&
       !value_is_block(target) && !value_is_block(arg)) {
	DEBUG_PRINTF(0,"\n");
	next_integer_operand(curr);
	next_integer_operand(curr);
	bytecode_boolean(value_get_integer(target)<value_get_integer(arg),
			 curr);
	return curr;
    } else {
	PUSH(curr,arg);
	return bytecode_do_m_call(curr,target);
    }   
}

static /*inline*/ struct activation_record *
bytecode_op_not(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Not: \n");
    value_t target = POP(curr);
    if(value_get_reference(target)==value_get_reference(bytecode_true)) {
	PUSH(curr,bytecode_false);
	next_integer_operand(curr);
	next_integer_operand(curr);
	return curr;
    } else if(value_get_reference(target)==
	      value_get_reference(bytecode_false)) {
	PUSH(curr,bytecode_true);
	next_integer_operand(curr);
	next_integer_operand(curr);
	return curr;
    }
    return bytecode_do_m_call(curr,target);
}

static /*inline*/ struct activation_record *
bytecode_op_between(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"Between: \n");
    value_t target = POP(curr);
    value_t arg2 = POP(curr);
    value_t arg1 = POP(curr);
    if(!value_is_reference(target) && !value_is_reference(arg1) &&
       !value_is_reference(arg2) && !value_is_block(target) &&
       !value_is_block(arg1) && !value_is_block(arg2)) {
	next_integer_operand(curr);
	next_integer_operand(curr);
	int64_t receiver = value_get_integer(target);
	bytecode_boolean(receiver>=value_get_integer(arg1) &&
			 receiver<=value_get_integer(arg2),
			 curr);
	return curr;
    } else {
	PUSH(curr,arg1);
	PUSH(curr,arg2);
	return bytecode_do_m_call(curr,target);
    }
}

static /*inline*/ struct activation_record *
bytecode_op_store_ret_val(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"StoreRetVal\n");
    curr->res = POP(curr);
    return curr;
}

static /*inline*/ struct activation_record *
bytecode_op_ret_val_return(struct activation_record *curr) {
    DEBUG_PRINTF(curr->depth,"RetValReturn\n");
    return bytecode_do_return(curr,curr->res);
}

void bytecode_run(struct activation_record *curr) {
    bytecount = 0;
    struct activation_record *last = curr;
    char *ptr = curr->pc-1;
    while(1) {
	if(curr != last) {
#if PROFILE
	    printf("CHANGETO: %s\n",try_translate_into_name(curr->code));
#endif
	    last->pc = ptr+1;
	    ptr = curr->pc;
	    last = curr;
	} else {
	    ptr++;
	}
	char instruction = *ptr;
#if COUNTER
	bytecount++;
#endif
#if PROFILE
	printf("INST\n");
#endif
	switch(instruction) {
	    case 0x00:
		curr = bytecode_op_finish(curr);
		break;
	    case 0x01:
		curr = bytecode_op_alloc(curr);
		break;
	    case 0x02:
		curr = bytecode_op_alloc_ind(curr);
		break;
	    case 0x03:
		curr = bytecode_op_arg(curr);
		break;
	    case 0x04:
		curr = bytecode_op_block(curr);
		break;
	    case 0x05:
		curr = bytecode_op_block_arg(curr);
		break;
	    case 0x06:
		curr = bytecode_op_call(curr);
		break;
	    case 0x07:
		curr = bytecode_op_ch_m_table(curr);
		break;
	    case 0x08:
		curr = bytecode_op_dup(curr);
		break;
	    case 0x09:
		curr = bytecode_op_equals(curr);
		break;
	    case 0x0A:
		curr = bytecode_op_false(curr);
		break;
	    case 0x0B:
		curr = bytecode_op_global(curr);
		break;
	    case 0x0C:
		curr = bytecode_op_inst(curr);
		break;
	    case 0x0D:
		curr = bytecode_op_inst_ind(curr);
		break;
	    case 0x0E:
		curr = bytecode_op_int(curr);
		break;
	    case 0x0F:
		curr = bytecode_op_is_rw(curr);
		break;
	    case 0x10:
		curr = bytecode_op_m_call(curr);
		break;
	    case 0x11:
		curr = bytecode_op_nil(curr);
		break;
	    case 0x12:
		curr = bytecode_op_pop(curr);
		break;
	    case 0x13:
		curr = bytecode_op_ref_equals(curr);
		break;
	    case 0x14:
		curr = bytecode_op_return(curr);
		break;
	    case 0x15:
		curr = bytecode_op_ro(curr);
		break;
	    case 0x16:
		curr = bytecode_op_rw(curr);
		break;
	    case 0x17:
		curr = bytecode_op_s_ch_m_table(curr);
		break;
	    case 0x18:
		curr = bytecode_op_secure(curr);
		break;
	    case 0x19:
		curr = bytecode_op_self(curr);
		break;
	    case 0x1A:
		curr = bytecode_op_s_inst(curr);
		break;
	    case 0x1B:
		curr = bytecode_op_s_inst_ind(curr);
		break;
	    case 0x1C:
		curr = bytecode_op_s_is_rw(curr);
		break;
	    case 0x1D:
		curr = bytecode_op_sl(curr);
		break;
	    case 0x1E:
		curr = bytecode_op_s_m_call(curr);
		break;
	    case 0x1F:
		curr = bytecode_op_s_return(curr);
		break;
	    case 0x20:
		curr = bytecode_op_s_ro(curr);
		break;
	    case 0x21:
		curr = bytecode_op_s_rw(curr);
		break;
	    case 0x22:
		curr = bytecode_op_s_secure(curr);
		break;
	    case 0x23:
		curr = bytecode_op_s_sl(curr);
		break;
	    case 0x24:
		curr = bytecode_op_store_global(curr);
		break;
	    case 0x25:
		curr = bytecode_op_store_inst(curr);
		break;
	    case 0x26:
		curr = bytecode_op_store_inst_ind(curr);
		break;
	    case 0x27:
		curr = bytecode_op_store_temp(curr);
		break;
	    case 0x28:
		curr = bytecode_op_s_store_inst(curr);
		break;
	    case 0x29:
		curr = bytecode_op_s_store_inst_ind(curr);
		break;
	    case 0x2A:
		curr = bytecode_op_swap(curr);
		break;
	    case 0x2B:
		curr = bytecode_op_temp(curr);
		break;
	    case 0x2C:
		curr = bytecode_op_temporaries(curr);
		break;
	    case 0x2D:
		curr = bytecode_op_true(curr);
		break;
	    case 0x2E:
		curr = bytecode_op_t_self(curr);
		break;
	    case 0x2F:
		curr = bytecode_op_t_s_m_call(curr);
		break;
	    case 0x30:
		curr = bytecode_op_alloc_from_args(curr);
		break;
	    case 0x31:
		curr = bytecode_op_store_arg(curr);
		break;
	    case 0x32:
		curr = bytecode_op_store_block_arg(curr);
		break;
	    case 0x33:
		curr = bytecode_op_block_evaluate(curr);
		break;
	    case 0x34:
		curr = bytecode_op_throw(curr);
		break;
	    case 0x35:
		curr = bytecode_op_block_args(curr);
		break;
	    case 0x36:
		curr = bytecode_op_add(curr);
		break;
	    case 0x37:
		curr = bytecode_op_subtract(curr);
		break;
	    case 0x38:
		curr = bytecode_op_multiply(curr);
		break;
	    case 0x39:
		curr = bytecode_op_divide(curr);
		break;
	    case 0x3A:
		curr = bytecode_op_mod(curr);
		break;
	    case 0x3B:
		curr = bytecode_op_greater(curr);
		break;
	    case 0x3C:
		curr = bytecode_op_less(curr);
		break;
	    case 0x3D:
		curr = bytecode_op_alloc_byte_array_ind(curr);
		break;
	    case 0x3E:
		curr = bytecode_op_store_into_byte_array(curr);
		break;
	    case 0x3F:
		curr = bytecode_op_index_byte_array_ind(curr);
		break;
	    case 0x40:
		curr = bytecode_op_between(curr);
		break;
	    case 0x41:
		curr = bytecode_op_not(curr);
		break;
	    case 0x42:
		curr = bytecode_op_integer_equals(curr);
		break;
	    case 0x43:
		curr = bytecode_op_store_ret_val(curr);
		break;
	    case 0x44:
		curr = bytecode_op_ret_val_return(curr);
		break;
	    default:
		fprintf(stderr,"Invalid byte -- %i\n",instruction);
		fprintf(stderr,"Method: %s",
			try_translate_into_name(curr->code));
		abort();
	}
    }
}
value_t bytecode_true;
value_t bytecode_false;
value_t bytecode_nil;

location_t block_object_mtable;
location_t smallinteger_mtable;

struct bytecode_block *initial_block;

char *file_start;
static off_t file_size;

#undef relocate
void *relocate(location_t offset) {
#if DEBUG
    if(offset > file_size) {
	fprintf(DEBUG_OUT,"Offset greater than file size relocated\n");
	abort();
    }
#endif
    return file_start + offset;
}

location_t derelocate(void *ptr) {
    return (char*)ptr - file_start;
}


value_t get_value(location_t location) {
    return *(value_t*)relocate(location);
}

void *get_pointer(location_t location) {
    return relocate(*(location_t *)relocate(location));
}

location_t get_location(location_t location) {
    return *(location_t *)relocate(location);
}

void process_file() {
    uint64_t *magic = relocate(0);
    if(magic[0] != 0xC0C03264AABBDDEEUL ||
       magic[1] != 0x0000000000000002UL) {
	fprintf(DEBUG_OUT,"insufficiently magical\n");
	abort();
    }
    bytecode_true = get_value(16);
    bytecode_false = get_value(32);
    bytecode_nil = get_value(48);
    block_object_mtable = get_location(64);
    smallinteger_mtable = get_location(72);
    initial_block = get_pointer(80);
}

void load_file(const char *file) {
    struct stat buf;
    int fd = open(file,O_RDONLY);
    if(fd<0) {
	fprintf(DEBUG_OUT,"file does not exist\n");
	abort();
    }
    off_t cur_size = 0;
    fstat(fd, &buf);
    file_size = buf.st_size;
    file_start = GC_MALLOC_UNCOLLECTABLE(file_size);
    while(cur_size < file_size) {
	int l = read(fd,file_start+cur_size,file_size-cur_size);
	if(l==0) {
	    fprintf(DEBUG_OUT,"EOF hit early - weird\n");
	    abort();
	}
	cur_size += l;
    }
    process_file();
}

const char *get_location_name(location_t location) {
    if(location==0 || location>=file_size) {
	return "0";
    }
    const uint64_t *region_ptr = relocate(location);
    size_t region_size = region_ptr[-1];
    region_ptr += region_size;
    const char *res = (const char*)region_ptr;
    return res;
}

const char *try_translate_into_name(void *ptr) {
    if((char*)ptr>=file_start && (char*)ptr<file_start+file_size) {
	return get_location_name(derelocate(ptr));
    } else {
	char *name = bytecode_malloc_no_pointers(16+2+1);
	sprintf(name,"0x%016x",ptr);
	return name;
    }
}

void run_initial_block() {
    struct activation_record *rec;
    struct execution_state *es;
    rec = activation_record_new(NULL,bytecode_nil,initial_block,NULL,0);
    bytecode_run(rec);
}

int main(int argc, char **argv) {
    if(argc != 2) {
	fprintf(DEBUG_OUT,"need a file to interpret\n");
	abort();
    }
    load_file(argv[1]);
    run_initial_block();
}
