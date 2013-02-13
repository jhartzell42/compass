#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "gc/gc.h"

#include "util.h"

#define MEMORY_ERROR "bytecode_malloc: out of memory\n"

static inline void out_of_memory() {
    write(2,MEMORY_ERROR,strlen(MEMORY_ERROR));
}

void eat_whitespace(char **ptr) {
    for(;isspace(**ptr);(*ptr)++)
	;
}

void skip_identifier(char **ptr) {
    for(;isalnum(**ptr) || **ptr == ':' || **ptr == '_';(*ptr)++)
	;
}

char *grab_identifier(char **ptr) {
    char *res = *ptr;
    skip_identifier(ptr);
    **ptr = '\0';
    return res;
}

void *bytecode_malloc(size_t size) {
    void *res;

    res = GC_MALLOC(size);

    if(res == NULL)
	out_of_memory();

    return res;
}

char *bytecode_strdup(const char *in) {
    char *res = bytecode_malloc(strlen(in)+1);
    strcpy(res,in);
    return res;
}

void *bytecode_realloc(void *old, size_t size) {
    void *res;

    res = GC_REALLOC(old, size); 

    if(res == NULL)
	out_of_memory();

    return res;
}

char *bytecode_afgets(FILE *stream) {
    char *buf;
    int cur=0;
    int eof=1;
    buf = bytecode_malloc(80);
    while(1) {
	if(fgets(buf+cur,80,stream)==NULL)
	    break;
	eof = 0;
	if(buf[strlen(buf)-1]=='\n')
	    break;
	cur = strlen(buf);
	buf = bytecode_realloc(buf,cur+80);
    }
    return eof?NULL:buf;
}
