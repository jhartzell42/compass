#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "value.h"
#include "util.h"
#include "memory_region.h"

struct value_t_output {
    unsigned securelevel: 3;
    bool writability: 1;
    bool reference: 1;
    bool relocated: 1;
    bool block: 1;
    unsigned code_location: 32;
    union {
	uint64_t i;
	void *ref;
    } contents;
};

/* notice that "referand" gets an @ appended here */
static void store_reference_to_memory_region(int securelevel,
					     bool writability,
					     const char *referand,
					     struct memory_region *mr,
					     int loc) {
    struct value_t_output output;
    output.writability = writability;
    output.securelevel = securelevel;
    output.reference = true;
    output.block = false;
    output.relocated = false;
    memcpy(&mr->data[loc],&output,sizeof(struct value_t_output));
    char *name = malloc(strlen(referand)+2);
    strcpy(name,"@");
    strcat(name,referand);
    /* TODO: who knows if this actually marks the right place? */
    mr_mark_name(mr, loc+1, name);
}

static void store_integer_value(int value,
				struct memory_region *mr,
				int loc) {
    mr->data[loc] = 0;
    mr->data[loc + 1] = value;
}

void process_value_decl(char *decl,
			char **end,
			struct memory_region *mr,
			int loc) {
    char *alloc, *ref, *name;
    alloc = ref = bytecode_strdup(decl);
    eat_whitespace(&ref);
    if(!strncmp(ref,"ref",3)) {
	ref+=3;
	eat_whitespace(&ref);
	if(*ref!='@') {
	    fprintf(stderr, "Invalid reference in value, no @: %s\n", ref);
	    abort();
	}
	ref++;
	name = grab_identifier(&ref);
	eat_whitespace(&ref);
	int securelevel = strtol(ref,&ref,10);
	if(!ref) {
	    fprintf(stderr, "Invalid securelevel in value.\n");
	    abort();
	}
	eat_whitespace(&ref);
	ref++; /*skip past "r"*/
	bool writability = (*ref=='w');
	store_reference_to_memory_region(securelevel,
					 writability,
					 name,
					 mr,
					 loc);
	ref++;
    } else if(!strncmp("int",ref,3)) {
	ref+=3;
	for(;isspace(*ref);ref++)
	    ;
	int value = strtol(ref,&ref,10);
	store_integer_value(value,mr,loc);
    } else {
	if(!strncmp("false",ref,5)) {
	    store_reference_to_memory_region(7,
					     false,
					     "false",
					     mr,
					     loc);
	    ref+=5;
	} else if(!strncmp("true",ref,4)) {
	    store_reference_to_memory_region(7,
					     false,
					     "true",
					     mr,
					     loc);
	    ref+=4;
	} else if(!strncmp("nil",ref,3)) {
	    store_reference_to_memory_region(7,
					     false,
					     "nil",
					     mr,
					     loc);
	    ref+=3;
	}
    }
    if(end!=NULL)
	*end = decl+(ref-alloc)+1;
}
