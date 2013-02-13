#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "object.h"
#include "util.h"
#include "memory_region.h"
#include "value.h"

void process_string_portion(char *line,void *whatever,
			    struct memory_region *region,
			    int where) {
    line++;
    for(int j=0;j<2;j++) {
	uint64_t val = 0;
	for(int i = 0; i < 8; i++) {
	    val |= ((uint64_t)strtol(line,&line,10)) << (i*8);
	    line++;
	    if(line == '\0') {
		break;
	    }
	}
	region->data[where+j] = val;
    }
}

struct memory_region *process_object_decl(char **object_decl, int nlines) {
    char *ref, *name, *table_name;
    struct memory_region *res;
    int cline;

    name = object_decl[0];
    res = mr_new(name,(nlines-2)*2+16);
    ref = object_decl[1];
    if(strncmp(ref,"mtable",6)) {
	for(cline = 1; cline<nlines; cline++) {
	    if(object_decl[cline][0] == 'S') {
		process_string_portion(object_decl[cline],NULL,
				       res,(cline-1)*2);
	    }
	}
	return res;
    }
    for(;*ref!='%';ref++)
	;
    table_name=ref;
    mr_mark_name(res,0,table_name);
    res->data[1] = nlines - 2;
    for(cline=2; cline<nlines;cline++) {
	process_value_decl(object_decl[cline],NULL,res,(cline-2)*2+2);
    }
    return res;
}
