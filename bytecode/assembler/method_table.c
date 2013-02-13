#include <stdint.h>
#include "memory_region.h"
#include "method_table.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "value.h"
#include "util.h"
#include "memory_region.h"
#include "selector.h"


static void process_line(struct memory_region *mr,
			 char *line,
			 int size) {
    char *selector_name; 
    int selector;
    int location;
    char *block_name;
    char *relocation_name;
    char *cur;
    cur = line;
    for(;*cur!='\'';cur++)
	;
    selector_name = ++cur;
    for(;*cur!='\'';cur++)
	;
    *cur = '\0';
    if(!strcmp(selector_name,"default:")) {
	;
    }
    selector_name = bytecode_strdup(selector_name);
    for(;*cur!='#';cur++)
	;
    cur++;
    block_name = grab_identifier(&cur);
    relocation_name = bytecode_malloc(strlen(block_name)+2);
    strcpy(relocation_name,"#");
    strcat(relocation_name,block_name);
    selector = resolve_selector(selector_name);
    location = (selector % size);
    for(;mr->data[(location+1)*2];location = (location + 1) % size)
	;
    mr->data[(location+1)*2] = selector;
    mr_mark_name(mr,(location+1)*2+1,relocation_name);
}

#define RATIO 8
int calculate_size(int nlines, int *log_size) {
    int size = 1;
    while(size<nlines*RATIO) {
	size <<= 1;
    }
    return size;
}

struct memory_region *
process_method_table_decl(char **decl, int nlines) {
    char *cur, *name, *default_block, *true_name;
    struct memory_region *mr;
    int i;
    cur = decl[0];
    eat_whitespace(&cur);
    cur++;
    name = grab_identifier(&cur);
    true_name = bytecode_malloc(strlen(name)+2);
    strcpy(true_name,"%");
    strcat(true_name,name);
    default_block = decl[1];
    for(default_block++;*default_block!='#';default_block++)
	;
    int size,log_size;
    size = calculate_size(nlines,&log_size);
    mr = mr_new(true_name,size*2 + 2);
    mr_mark_name(mr,1,default_block);
    mr->data[0] = size-1;
    for(i=2;i<nlines;i++) {
	process_line(mr,decl[i],size);
    }
    return mr;
}
