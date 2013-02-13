#ifndef BYTECODE_MEMORY_REGION_H
#define BYTECODE_MEMORY_REGION_H
#define RELOCATIONS_PER_REGION 512
#include <stdint.h>
#include <unistd.h>
struct memory_region {
    int name_locations[RELOCATIONS_PER_REGION];
    int name_location_size;
    int reference_locations[RELOCATIONS_PER_REGION];
    int reference_location_size;
    int size;
    uint64_t final_location;
    uint64_t data[0];
};

struct memory_region *mr_new(const char *name, size_t size);

struct memory_region *mr_resize(struct memory_region *old,
				int new_size);

void mr_mark_name(struct memory_region *mr, int loc, const char *name);
void mr_mark_ptr(struct memory_region *mr,
		 int loc,
		 struct memory_region *ptr);
void relocate_regions(void **result,
		      uint64_t *size,
		      struct memory_region *header);
#endif
