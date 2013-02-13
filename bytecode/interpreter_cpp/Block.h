#ifndef BYTECODE_BLOCK_H
#define BYTECODE_BLOCK_H

#include "file.h"
#include "Value.h"
#include <iostream>

#define BYTECODE = 0

class MethodTable;

struct OperandList {
    uint64_t operands[0];
};

struct BlockPosition {
    int current_instruction;
    int current_operand;
};

enum OperandType {
    NIL,
    INT,
    SELECTOR,
    BLOCK,
    GLOBAL,
    METHOD_TABLE,
};

class Block {
    private:
	uint64_t type;
	Location<OperandList> operands;
	int pad;
	char instructions[0];
	Block();
	static const char *const bytecodes[];
	static const OperandType operand_descriptions[][10];
	friend std::ostream &operator<<(std::ostream &, const Block &);
	friend BlockPosition
	    display_current_instruction(std::ostream &,
					const Block &,
					BlockPosition pos);
	friend class ActivationRecord;
	Location<Value> get_global_argument(BlockPosition &pos);
	Location<Block> get_block_argument(BlockPosition &pos);
	Location<MethodTable> get_method_table_argument(BlockPosition &pos);
	uint64_t get_integer_argument(BlockPosition &pos);
};

#endif
