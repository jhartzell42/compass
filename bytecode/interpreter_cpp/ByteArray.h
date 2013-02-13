#ifndef BYTECODE_BYTE_ARRAY_H
#define BYTECODE_BYTE_ARRAY_H

#include <string>

struct ByteArrayAllocation {
    int refs;
    char arr[0];
};

class ByteArray {
    public:
	// XXX should be private -- fix as soon as StoreIntoByteArray
	// is removed
	const char *start;
	const char *end;
	ByteArrayAllocation *alloc;
	ByteArray(const char *cstring, int len);
	ByteArray(ByteArrayAllocation *alloc, int off1, int off2);
	ByteArray(const ByteArray &old_ref);
	ByteArray();
	~ByteArray();
	int size() const;
	operator std::string() const;
	const char &operator[](int index) const;
};

ByteArray byte_array_slice(const ByteArray &old,
			   int slice_start,
			   int slice_end);

void incref(ByteArrayAllocation *alloc);
void decref(ByteArrayAllocation *alloc);
ByteArrayAllocation *alloc_byte_array(size_t sz);
#endif
