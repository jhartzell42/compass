#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "file.h"
#include "Block.ih"
#include "Value.ih"
#include "MethodTable.ih"
#include "globals.h"

char *file_start;
int file_size;

Location<Value> bytecode_true;
Location<Value> bytecode_false;
Location<Value> bytecode_nil;

Location<MethodTable> block_object_mtable;
Location<MethodTable> smallinteger_mtable;

Location<Block> initial_block;

void process_file() {
    Location<uint64_t> magic;
    Location<uint64_t> version;
    magic.set_offset(sizeof(uint64_t)*0);
    version.set_offset(sizeof(uint64_t)*1);
    if(*magic != 0xC0C03264AABBDDEEUL &&
       *version != 0x00000000000000002) {
	std::cerr << "Insufficiently magical" << std::endl;
	abort();
    }
    bytecode_true.set_offset(16);
    bytecode_false.set_offset(32);
    bytecode_nil.set_offset(48);
    block_object_mtable.set_offset(64);
    smallinteger_mtable.set_offset(72);
    Location<Location<Block> > initial_block_field;
    initial_block_field.set_offset(80);
    initial_block = *initial_block_field;
}

void load_file(const char *filename) {
    struct stat buf;
    int fd = open(filename,O_RDONLY);
    if(fd<0) {
	std::cerr << "file " << filename << " does not exist" << std::endl;
	abort();
    }
    off_t cur_size = 0;
    fstat(fd, &buf);
    file_size = buf.st_size;
    // TODO -- replace with mmap
    file_start = new char[file_size];
    while(cur_size < file_size) {
	int l = read(fd, file_start + cur_size, file_size-cur_size);
	if(l==0) {
	    std::cerr << "EOF hit early -- weird" << std::endl;
	    abort();
	}
	cur_size += l;
    }
    process_file();
}
