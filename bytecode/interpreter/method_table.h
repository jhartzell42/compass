#ifndef BYTECODE_METHOD_TABLE_H
#define BYTECODE_METHOD_TABLE_H
#include <sys/types.h>
#include <stdint.h>

#include "file.h"

typedef uint64_t selector_t;

struct method_table_entry {
    selector_t sel;
    location_t block_offset;
};

struct bytecode_method_table {
    size_t size;
    location_t default_block_offset;
    struct method_table_entry entries[0];
};

#endif
