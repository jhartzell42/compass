#ifndef BYTECODE_VALUE_H
#define BYTECODE_VALUE_H

#include <stdlib.h>
#include <stdint.h>

#include "file.h"
#include "gc.h"

class Object;
class ObjectAllocation;
class MethodInvocation;
class Block;

typedef unsigned secure_t;
class Value {
    private:
	secure_t securelevel: 3;
	bool writability: 1;
	bool reference: 1;
	bool on_heap: 1;
	bool block: 1;
	Location<Block> code;
	union {
	    int64_t i;
	    Location<Object> image_object;
	    ObjectAllocation *heap_object;
	    MethodInvocation *method_invocation;
	};
	void perform_inc();
	void perform_dec();
	Object &get_bytecode_object() const;
    public:
	GCAllocation *get_any_gc_reference() const;	
	Value(const Value &old);
	Value(int64_t new_i = 0);
	Value(int new_i);
	Value(GCReference<ObjectAllocation> &new_heap_object,
	      secure_t new_sl = 0, bool new_writability = true);
	Value(ObjectAllocation *new_heap_object,
	      secure_t new_sl = 0, bool new_writability = true);
	Value(Location<Block> code, GCReference<MethodInvocation> closure);
	Value(bool b);
	Object &operator*() const;
	Object *operator->() const;
	operator int64_t() const;
	Value &operator=(const Value &old);
	bool operator==(const Value &other) const;
	bool operator!=(const Value &other) const;
	bool is_integer() const;
	bool is_reference() const;
	bool is_block() const;
	bool is_on_heap() const;
	Location<Object> get_object_location() const;
	secure_t get_securelevel() const;
	void set_securelevel(secure_t new_securelevel);
	bool get_writability() const;
	void set_writability(bool b);
	Location<Block> get_block_code() const;
	MethodInvocation *get_closure_method_invocation() const;
	void invoke_garbage_collection_method(GCMemFn ch) const;
};
#endif
