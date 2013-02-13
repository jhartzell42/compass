#include "memory_region.h"
#include "util.h"
#include "hash_table.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define WORD_COUNT_TO_BYTE_COUNT(x) ((x)<<3)

bytecode_hash_table region_names;
static bool initialized = false;

static inline void try_initialize() {
    if(!initialized) {
	bytecode_hash_table_init(&region_names);
	initialized = true;
    }
}

struct memory_region *mr_new(const char *name, size_t size) {
    struct memory_region *res;
    try_initialize();
    if(size == 0) {
	fprintf(stderr,"Sizes of memory regions must be non-zero\n");
	fprintf(stderr,"%s violates that constraint, you evil person\n",
		name);
	abort();
    }
    res = bytecode_malloc(sizeof(struct memory_region) +
			    size*sizeof(uint64_t));
    res->name_location_size = 0;
    res->reference_location_size = 0;
    res->size = size;
    res->final_location = -1;
    bytecode_hash_table_ins(&region_names,name,res);
    if(name == NULL || name[0] == '\0') {
	fprintf(stderr,"memory regions must have a name\n");
	abort();
    }
    return res;
}

struct memory_region *mr_resize(struct memory_region *old, int size) {
    struct memory_region *res;
    res = bytecode_realloc(old, sizeof(struct memory_region) +
			       size*sizeof(uint64_t));
    res->size = size;
    return res;
}

void mr_mark_name(struct memory_region *mr, int loc, const char *name) {
    if(name == NULL || name[0] == '\0') {
	fprintf(stderr,"marking a name must provide it a name\n");
	abort();
    }
    mr->data[loc] = (uint64_t)bytecode_strdup(name);
    mr->name_locations[mr->name_location_size++] = loc;
}

void mr_mark_ptr(struct memory_region *mr,
		 int loc,
		 struct memory_region *ptr) {
    mr->data[loc] = (uint64_t)ptr;
    mr->reference_locations[mr->reference_location_size++] = loc;
}

#define DEBUG_WORDS 16
/* 16 words should be enough for anybody - debugging info */

void determine_location(void *data,
			const char *restrict ignore,
			void *region_ptr) {
    struct memory_region *mr = region_ptr;
    uint64_t *cur_pos = data;
    if(mr->final_location == -1) {
	mr->final_location = *cur_pos+1;
	*cur_pos += mr->size + DEBUG_WORDS;
    }
}


static void relocate_region(void *data,
		     const char *restrict ignore,
		     void *region_ptr) {
    int i;
    struct memory_region *mr = region_ptr;
    for(i=0; i<mr->name_location_size; i++) {
	int loc = mr->name_locations[i];
	const char *name = (const char *)mr->data[loc];
	const struct memory_region *referand =
	    bytecode_hash_table_get(&region_names,
				    name);
	if(referand == NULL) {
	    fprintf(stderr,"%s not existant\n",name);
	    abort();
	}
	mr->data[loc] = WORD_COUNT_TO_BYTE_COUNT(referand->final_location);
    }
    for(i=0; i<mr->reference_location_size; i++) {
	int loc = mr->reference_locations[i];
	const struct memory_region *referand =
	    (const struct memory_region *)mr->data[loc];
	mr->data[loc] = WORD_COUNT_TO_BYTE_COUNT(referand->final_location);
    }
}

static void output_data(void *data,
			const char *name,
			void *region_ptr) {
    uint64_t *res = data;
    struct memory_region *mr = region_ptr;
    res[mr->final_location-1] = mr->size;
    memcpy(res + mr->final_location,
	   mr->data,
	   mr->size * sizeof(uint64_t));
    void *debug_location = res + mr->final_location + mr->size;
    strcpy(debug_location, name);
}

void relocate_regions(void **reg,
		      size_t *size,
		      struct memory_region *header) {
    /* determine where the regions go */
    header->final_location = 0;
    uint64_t cur_pos = header->size + DEBUG_WORDS;
    bytecode_hash_table_iter(&region_names,determine_location,&cur_pos);
    bytecode_hash_table_iter(&region_names,relocate_region,NULL);
    uint64_t *res = bytecode_malloc(cur_pos * sizeof(uint64_t));
    bytecode_hash_table_iter(&region_names,output_data,res);
    *reg = res;
    *size = WORD_COUNT_TO_BYTE_COUNT(cur_pos);
}
