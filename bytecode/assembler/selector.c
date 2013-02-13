#include "util.h"
#include "hash_table.h"
int current_selector = 0x1000;

bytecode_hash_table symbol_table;
static bool initialized = false;

static void try_initialize() {
    if(!initialized) {
	initialized = true;
	bytecode_hash_table_init(&symbol_table);
    }
}

static void check_for_quotes(const char *sel) {
    int i;
    for(i=0;sel[i];i++) {
	if(sel[i] == '\'') {
	    fprintf(stderr,"No quotes allowed in selector names\n");
	    fprintf(stderr,"Like %s\n",sel);
	    abort();
	}
    }
}

static int new_selector(const char *sel) {
    int *res = bytecode_malloc(sizeof(int));
    check_for_quotes(sel);
    *res = current_selector++;
    if(sel == NULL || sel[0] == '\0') {
	fprintf(stderr,"Empty or null selectors not allowed\n");
	abort();
    }
    bytecode_hash_table_ins(&symbol_table,sel,res);
    return *res;
}

int resolve_selector(const char *sel) {
    try_initialize();
    if(sel == NULL || sel[0] == '\0') {
	fprintf(stderr,"Empty or null selectors not allowed\n");
	abort();
    }
    int *res = bytecode_hash_table_get(&symbol_table,sel);
    if(res==NULL) {
	return new_selector(sel);
    } else {
	return *res;
    }
}
