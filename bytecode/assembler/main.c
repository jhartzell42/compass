#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parse.h"
#include "value.h"
#include "util.h"

int main(int argc, char** argv) {
    FILE *file;
    if(argc < 2) {
	fprintf(stderr, "Usage: %s FILE [INCLUDE_DIR]\n", argv[0]);
	abort();
    } 
    file = fopen(argv[1], "r");
    if(argc == 3) {
	chdir(argv[2]);
    }

    parse_bytecode(file);
    struct memory_region *init = mr_new("initializer",0x20);
    init->data[0] = 0xC0C03264AABBDDEEUL;
    init->data[1] = 0x0000000000000002UL;

    char *empty;
    process_value_decl(bytecode_strdup("ref @true 7 ro"),&empty,init,2);
    process_value_decl(bytecode_strdup("ref @false 7 ro"),&empty,init,4);
    process_value_decl(bytecode_strdup("ref @nil 7 ro"),&empty,init,6);
    mr_mark_name(init,8,"%BlockLiteral");
    mr_mark_name(init,9,"%SmallInteger");
    mr_mark_name(init,10,"#initialize");

    void *to_output;
    size_t size;
    relocate_regions(&to_output,&size,init);
    write(1,to_output,size);
}
