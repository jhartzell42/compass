#ifndef ASSEMBLER_VALUE_H
#define ASSEMBLER_VALUE_H
#include "memory_region.h"

void process_value_decl(char *decl, /* start of value decl. */
			char **end, /* OUT. end of decl. */
			struct memory_region *mr,
			int loc);   /* place to put processed value */
#define VALUE_WORDS 2
#endif
