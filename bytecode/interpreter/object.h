#ifndef BYTECODE_OBJECT_H
#define BYTECODE_OBJECT_H
#include "value.h"
#include "method_table.h"
struct bytecode_object {
    location_t mtable;
    uint64_t ivarc;
    value_t ivars[0];
};

struct bytecode_object *object_new(size_t ivars);

location_t
object_get_table(struct bytecode_object *obj);

void object_set_table(struct bytecode_object* obj,
		      location_t tab);

value_t object_get_ivar(struct bytecode_object *obj, int num);

void object_set_ivar(struct bytecode_object *obj, int num, value_t val);

#endif
