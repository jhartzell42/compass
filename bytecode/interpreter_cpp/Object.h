#ifndef BYTECODE_OBJECT_H
#define BYTECODE_OBJECT_H

#include "gc.h"
#include "Value.h"
#include "file.h"
#include "Value.h"
#include "ByteArray.h"

#include <stdint.h>
class MethodTable;

class Object {
    private:
	friend class ObjectAllocation;
	void *operator new(size_t base_size, uint64_t ivarc);
	void operator delete(void *alloc);
	explicit Object(uint64_t new_ivarc);
	Location<MethodTable> method_table;
	int padding;
	uint64_t ivarc;
	Value ivars[0];
	void for_each_child(GCMemFn ch);
	void set_all_refs_to_zero();
    public:
	Value &operator[](int which);
	const Value &operator[](int which) const;
	int size() const;
	MethodTable &get_method_table() const;
	Location<MethodTable> get_method_table_location() const;
	void set_method_table(Location<MethodTable> new_method_table);
	ByteArray get_byte_array();
	void set_byte_array(ByteArray &arr);
	~Object();
};

class ObjectAllocation : public virtual GCAllocation { 
    private:
	friend class Value;
	Object *contents;
	ObjectAllocation(uint64_t ivarc);
	~ObjectAllocation();
    public:
	static ObjectAllocation *allocate_object(uint64_t ivarc);
	static ObjectAllocation *allocate_object(ByteArray &arr);
	virtual void deallocate_self();
	virtual void for_each_child(GCMemFn ch);
	virtual void set_all_refs_to_zero();
};
#endif
