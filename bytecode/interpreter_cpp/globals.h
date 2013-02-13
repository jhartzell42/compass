#ifndef BYTECODE_GLOBALS_H
#define BYTECODE_GLOBALS_H
#include "file.h"
#include "Value.h"
#include "Block.h"
#include "MethodTable.h"
extern Location<Value> bytecode_true;
extern Location<Value> bytecode_false;
extern Location<Value> bytecode_nil;

extern Location<MethodTable> block_object_mtable;
extern Location<MethodTable> smallinteger_mtable;

extern Location<Block> initial_block;
#endif
