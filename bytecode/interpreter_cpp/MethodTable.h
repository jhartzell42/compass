#ifndef BYTECODE_METHOD_TABLE_H
#define BYTECODE_METHOD_TABLE_H

#include "Block.h"

typedef uint64_t selector_t;
class MethodTableEntry {
    private:
	friend class MethodTable;
	selector_t sel;
	Location<Block> block;
	int padding;
};

class MethodTable {
    private:
	selector_t size_mask;
	Location<Block> default_block;
	MethodTableEntry entries[0];
	MethodTable();
    public:
	const Location<Block> &operator[](selector_t sel) const;
};
#endif
