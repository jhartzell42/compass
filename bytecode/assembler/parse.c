#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "object.h"
#include "method_table.h"
#include "global.h"
#include "util.h"
#include "parse.h"
#include "block.h"

static char *trim(char *str) {
    char *res, *end;
    res = str;
    eat_whitespace(&res);
    for(end = res + strlen(res) - 1;isspace(*end) && end>res;end--)
	;
    *(end+1) = '\0';
    return res;
}

static char *comment_trim(char *str) {
    char *end;
    for(end=str;*end!='\0' && *end!='\"';end++)
	;
    *end='\0';
    return str;
}

static void dispatch_lines(char **lines, int nlines) {
    switch(lines[0][0]) {
	case '%':
	    process_method_table_decl(lines,nlines);
	    break;
	case '#':
	    process_block_decl(lines,nlines);
	    break;
	case '$':
	    if(nlines!=1) {
		fprintf(stderr, "Parse error: global declarations must"
			"be single lines.\n");
		abort();
	    }
	    process_global_decl(lines[0]);
	    break;
	case '<':
	    if(nlines!=1) {
		fprintf(stderr, "Parse error: import declarations must"
			"be single lines.\n");
		abort();
	    }
	    const char* import_fname = trim(lines[0]+1);
	    FILE* import_file = fopen(import_fname, "r");
	    if(import_file != NULL) {
		parse_bytecode(import_file);
	    } else {
		fprintf(stderr,
		    "Failed to load imported file %s.");
		abort();
	    } 
	    break;
	case '@':
	    process_object_decl(lines,nlines);
	    break;
	default:
	    fprintf(stderr, "dispatch_lines switch default\n");
	    fprintf(stderr, "Bad line %s\n",lines[0]);
	    abort();
    }
}

void parse_bytecode(FILE *program) {
    char **lines=0;
    int nlines=0;
    while(!feof(program)) { 
	lines = bytecode_realloc(lines,(nlines++)*sizeof(char*));
	lines[nlines-1] = bytecode_afgets(program);
	if(lines[nlines-1]==0) {
	    nlines--;
	    break;
	}
	lines[nlines-1] = comment_trim(lines[nlines-1]);
	lines[nlines-1] = trim(lines[nlines-1]);
	if(lines[nlines-1][0]=='\0') { /*all whitespace*/
	    nlines--;
	    continue;
	}
	switch(lines[nlines-1][0]) {
	    case '%': case '#': case '@': case '<': case '$':
		if(nlines!=1) {
		    dispatch_lines(lines,nlines-1);
		}
		lines[0]=lines[nlines-1];
		nlines=1;
		break;
	}
    }
    dispatch_lines(lines,nlines);
}
