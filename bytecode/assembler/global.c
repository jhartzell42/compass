#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "value.h"
#include "hash_table.h"
#include "util.h"

struct memory_region *process_global_decl(char *global_decl) {
    char *name, *decl, *alloc;
    struct memory_region *res;
    alloc = decl = bytecode_strdup(global_decl);
    /* get global name */
    for(;*decl != '$';decl++)
	;
    decl++;
    eat_whitespace(&decl);
    name = grab_identifier(&decl);
    char *sigiled_name = bytecode_malloc(strlen(name) + 2);
    strcpy(sigiled_name,"$");
    strcat(sigiled_name,name);
    /* create memory region */
    res = mr_new(sigiled_name,VALUE_WORDS); 
    /* get rest of global line */
    decl++;
    for(;isspace(*decl) || *decl=='=';decl++)
	;
    process_value_decl(decl,NULL,res,0);
    return res;
}
