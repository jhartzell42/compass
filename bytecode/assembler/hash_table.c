#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gc/gc.h"

#include "util.h"
#include "hash_table.h"

/** Initial size for hash table allocations. */
#define BYTECODE_INIT_ALLOC 64

/** Big prime for computing hashes. */
#define BYTECODE_BIG_PRIME 6291469UL

/** Maximum load value for the hash table. */
#define BYTECODE_MAX_LOAD 0.5

void bytecode_hash_table_init(bytecode_hash_table *restrict h)
{
    h->alloc = BYTECODE_INIT_ALLOC;
    h->nmemb = 0;
    h->arr = bytecode_malloc(sizeof (*h->arr)*BYTECODE_INIT_ALLOC);

    for (size_t i = 0; i < h->alloc; ++i) {
	h->arr[i].key = NULL;
	h->arr[i].value = NULL;
    }
}

static size_t bytecode_hash(const char *restrict key, size_t alloc) {
    size_t hash = 0;

    while (*key != '\0') {
	hash += (hash << 5) + *key++;
    }
    hash *= BYTECODE_BIG_PRIME;

    return hash % alloc;
}

static void bytecode_hash_table_double(bytecode_hash_table *restrict h) {
    bytecode_hash_table tmp;
    size_t i;

    tmp.alloc = h->alloc * 2;
    tmp.nmemb = h->nmemb;
    tmp.arr = bytecode_malloc(tmp.alloc*sizeof (*tmp.arr));

    for (size_t j = 0; j < tmp.alloc; ++j) {
	tmp.arr[j].value = NULL;
	tmp.arr[j].key = NULL;
    }

    for (i = 0; i < h->alloc; ++i) {
	if (h->arr[i].key != NULL) {
	    bytecode_hash_table_ins(&tmp, h->arr[i].key, h->arr[i].value);
	}
    }

    h->arr = tmp.arr;
    h->alloc = tmp.alloc;
}

bool bytecode_hash_table_ins(bytecode_hash_table *restrict h,
			     const char *key,
			     void *value) {
    if(key == NULL || key[0] == '\0') {
	fprintf(stderr,"Cannot have null or empty key\n");
	abort();
    }
    ++h->nmemb;
    while (((float)h->nmemb) / h->alloc > BYTECODE_MAX_LOAD) {
	bytecode_hash_table_double(h);
    }

    size_t hash = bytecode_hash(key, h->alloc);
    for (;;) {
	if (h->arr[hash].key == NULL) {
	    h->arr[hash].key = bytecode_strdup(key);
	    h->arr[hash].value = value;
	    return true;
	} else if (strcmp(key, h->arr[hash].key) == 0) {
	    h->arr[hash].value = value;
	    return false;
	} else if (hash == h->alloc - 1) {
	    hash = 0;
	} else {
	    ++hash;
	}
    }
}

void * bytecode_hash_table_get(const bytecode_hash_table *restrict h,
			       const char *restrict key) {
    size_t hash, start;

    start = hash = bytecode_hash(key, h->alloc);
    for (;;) {
	if (h->arr[hash].key == NULL) {
	    return NULL;
	} else if (strcmp(h->arr[hash].key, key) == 0) {
	    return h->arr[hash].value;
	} else if (hash == h->alloc - 1) {
	    hash = 0;
	} else {
	    ++hash;
	}
    }
}

/*not copyright Trevor Caira*/
void bytecode_hash_table_iter(bytecode_hash_table *restrict h,
			 void (*iterfunc)(void*, const char*, void*),
			 void *data) {
    size_t hash;
    for(hash = 0; hash < h->alloc; hash++) {
	if (h->arr[hash].key != NULL) {
	    //printf("%s\n",h->arr[hash].key);
	    iterfunc(data, h->arr[hash].key, h->arr[hash].value);
	}
    }
}
