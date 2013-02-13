#ifndef BYTECODE_FILE_H
#define BYTECODE_FILE_H

#include "debug.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

extern char *file_start;
extern int file_size;

template <class T>
class Location {
    private:
	int offset;
    public:
	void set_offset(int new_offset) {
	    offset = new_offset;
	}

	operator bool() const {
	    return offset;
	}

	T *get_pointer() const {
	    // assert(offset>0 && offset<file_size);
	    if(offset < 0 || offset >= file_size) {
		std::cout << "Error accessing object: "<< this << std::endl;
		std::cout << "Offset " << offset << " not contained below "
		    << file_size;
		abort();
	    }
	    return ((T*)(file_start + offset));
	}

	T &operator*() const {
	    return *get_pointer();
	}

	T *operator->() const {
	    return get_pointer();
	}

	const char *get_name() const {
	    assert(offset > 0 && offset < file_size);
	    const uint64_t *region_ptr = (const uint64_t*)get_pointer();
	    size_t region_size = region_ptr[-1];
	    region_ptr += region_size;
	    const char *res = (const char*)region_ptr;
	    return res;
	}

	bool operator==(Location<T> &other) {
	    return offset == other.offset;
	}
};

template <class T>
inline std::ostream &operator<<(std::ostream &os, const Location<T> &loc) {
    os << loc.get_name();
}

void load_file(const char *filename);
#endif
