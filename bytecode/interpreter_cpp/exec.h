#ifndef BYTECODE_EXEC_H
#define BYTECODE_EXEC_H

#include "gc.h"
#include "Block.h"
#include "Value.h"
#include "MethodTable.h"

#include <vector>

#define STACK_SIZE 12

class ActivationRecord;
class MethodInvocation : public virtual GCAllocation {
    private:
	friend class ActivationRecord;
	std::vector<Value> args;
	std::vector<Value> temps;
	Value res;
	GCReference<ActivationRecord> ret;
	bool valid_return;
    public:
	void for_each_child(GCMemFn ch);
	void set_all_refs_to_zero();
	template <class InputIterator>
	    MethodInvocation(ActivationRecord *ret,
			     InputIterator args_begin,
			     InputIterator args_end);
	MethodInvocation(ActivationRecord *ret);
	void allocate_temps(int sz);
};

class ActivationRecord : public virtual GCAllocation {
    private:
	friend void bytecode_run(GCReference<ActivationRecord> curr);
	Location<Block> code;
	BlockPosition pos;
	std::vector<Value> block_args;
	std::vector<Value> stack;
	Value self;
	GCReference<MethodInvocation> meth;
	GCReference<ActivationRecord> block_ret;
	bool is_repeat;
	bool is_method;
#if DEBUG
	bool deleted;
#endif
	Value catch_block;
	ActivationRecord(const ActivationRecord&) {
	    assert(false);
	}
    public:
	ActivationRecord(Location<Block> code,
			 Value receiver, 
			 GCReference<MethodInvocation> meth,
			 GCReference<ActivationRecord> block_ret,
			 bool is_repeat,
			 bool is_method,
			 Value catch_block);
	template <class InputIterator>
	    ActivationRecord(Location<Block> code,
			 Value receiver, 
			 GCReference<MethodInvocation> meth,
			 GCReference<ActivationRecord> block_ret,
			 bool is_repeat,
			 bool is_method,
			 Value catch_block,
			 InputIterator block_args_start,
			 InputIterator block_args_end);
	void for_each_child(GCMemFn ch);
	void set_all_refs_to_zero();
	void push(const Value &to_push);
	Value pop();
	Value &top_of_stack();
	const Value &top_of_stack() const;
	void set_top_of_stack(Value val);
	ActivationRecord *execute_instruction();
	ActivationRecord *call(Location<Block> blk, int argc,
			       const Value &receiver);
	ActivationRecord *method_call(selector_t blk, int argc,
			       const Value &receiver);
	ActivationRecord *do_return(Value to_return);
	ActivationRecord *call_builtin(Location<Block> blk,
				       int argc,
				       const Value &receiver);
	~ActivationRecord();
	ActivationRecord *evaluate_block(Value blk, int argc);
};

void bytecode_run(GCReference<ActivationRecord> curr);
#endif
