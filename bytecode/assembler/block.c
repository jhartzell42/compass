#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memory_region.h"
#include "hash_table.h"
#include "block.h"
#include "util.h"
#include "selector.h"
#include "parse.h"
#include "builtins.h"

extern const char *bytecodes[];
static bool initialized;

static char get_bytecode(char **decl);

static void process_argument(struct memory_region *operands,
		      int *current_operand,
		      char **c);

static inline void initialize(void) {
    if(!initialized) {
	add_all_builtins();
	initialized = true;
    }
}

static int operand_region_counter = 0;
void process_block_decl(char **decl, int nlines) {
    char *name,*cur,*region_name,operand_name[40];
    struct memory_region *bytes, *operands;
    initialize();
    sprintf(operand_name,"ops%i",operand_region_counter++);

    cur = decl[0]+1;
    eat_whitespace(&cur);
    name=grab_identifier(&cur);
    region_name = bytecode_malloc(strlen(name)+2);
    strcpy(region_name,"#");
    strcat(region_name,name);

    bytes = mr_new(region_name,3 + (nlines+17)/8);
    operands = mr_new(operand_name,nlines*2);
    mr_mark_ptr(bytes,1,operands);
    bytes->data[0] = 0;
    int current_operand = 0;
    int current_byte = 0;
    int current_word = 2;
    int i;
    uint64_t word = 0;
    char *byte = (char *)&word;
    for(i=1;i<nlines;i++) {
	char *cur_line = decl[i];
	*byte++ = get_bytecode(&cur_line);
	cur_line++;
	eat_whitespace(&cur_line);
	while(*cur_line != '\0') {
	    process_argument(operands,&current_operand,&cur_line);
	    eat_whitespace(&cur_line);
	}
	if(++current_byte==8) {
	    current_byte = 0;
	    bytes->data[current_word] = word;
	    byte = (char *)&word;
	    current_word++;
	    word = 0;
	}
    }
    bytes->data[current_word] = word;
    bytes->data[current_word+1] = 0;
    *byte = 0;
}

void process_argument(struct memory_region *operands,
		      int *current_operand,
		      char **c) {
    char *selector;
    char sigil;
    switch(**c) {
	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	    operands->data[(*current_operand)++] =
		strtol(*c,c,10);
	    break;
	case '\'':
	    selector = ++*c;
	    for(;**c != '\'';(*c)++)
		;
	    **c = '\0';
	    (*c)++;
	    operands->data[(*current_operand)++] =
		resolve_selector(selector);
	    break;
	case '#':
	case '@':
	case '$':
	case '%':
	    sigil = **c;
	    (*c)++;
	    char *name = grab_identifier(c);
	    char *name_with_sigil = bytecode_malloc(strlen(name) + 2);
	    name_with_sigil[0] = sigil;
	    name_with_sigil[1] = '\0';
	    strcat(name_with_sigil,name);
	    mr_mark_name(operands,(*current_operand)++,name_with_sigil);
	    break;
	default:
	    fprintf(stderr,"hit operand default: %s\n",*c);
	    abort();
    }
}

char get_bytecode(char **decl) {
    char *id = grab_identifier(decl);
    char i;
    for(i=0;bytecodes[i]!=NULL;i++) {
	if(!strcmp(bytecodes[i],id)) {
	    return i;
	}
    }
    fprintf(stderr,"Illegal instruction: %s",id);
    abort();
}

const char *bytecodes[] = {
    "FINISH",	    /*0x00*/
    "Alloc",	    /*0x01*/
    "AllocInd",	    /*0x02*/
    "Arg",	    /*0x03*/
    "Block",	    /*0x04*/
    "BlockArg",	    /*0x05*/
    "Call",	    /*0x06*/
    "ChMTable",	    /*0x07*/
    "Dup",	    /*0x08*/
    "Equals",	    /*0x09*/
    "False",	    /*0x0A*/
    "Global",	    /*0x0B*/
    "Inst",	    /*0x0C*/
    "InstInd",	    /*0x0D*/
    "Int",	    /*0x0E*/
    "IsRw",	    /*0x0F*/
    "MCall",	    /*0x10*/
    "Nil",	    /*0x11*/
    "Pop",	    /*0x12*/
    "RefEquals",    /*0x13*/
    "Return",	    /*0x14*/
    "Ro",	    /*0x15*/
    "Rw",	    /*0x16*/
    "SChMTable",    /*0x17*/
    "Secure",	    /*0x18*/
    "Self",	    /*0x19*/
    "SInst",	    /*0x1A*/
    "SInstInd",	    /*0x1B*/
    "SIsRw",	    /*0x1C*/
    "Sl",	    /*0x1D*/
    "SMCall",	    /*0x1E*/
    "SReturn",	    /*0x1F*/
    "SRo",	    /*0x20*/
    "SRw",	    /*0x21*/
    "SSecure",	    /*0x22*/
    "SSl",	    /*0x23*/
    "StoreGlobal",  /*0x24*/
    "StoreInst",    /*0x25*/
    "StoreInstInd", /*0x26*/
    "StoreTemp",    /*0x27*/
    "SStoreInst",   /*0x28*/
    "SStoreInstInd",/*0x29*/
    "Swap",	    /*0x2A*/
    "Temp",	    /*0x2B*/
    "Temporaries",  /*0x2C*/
    "True",	    /*0x2D*/
    "TSelf",	    /*0x2E*/
    "TSMCall",	    /*0x2F*/
    "AllocFromArgs",/*0x30*/
    "StoreArg",	    /*0x31*/
    "StoreBlockArg",/*0x32*/
    "BlockEvaluate",/*0x33*/
    "Throw",	    /*0x34*/
    "BlockArgs",    /*0x35*/
    "Add",	    /*0x36*/
    "Subtract",	    /*0x37*/
    "Multiply",	    /*0x38*/
    "Divide",	    /*0x39*/
    "Mod",	    /*0x3A*/
    "Greater",	    /*0x3B*/
    "Less",	    /*0x3C*/
    "AllocByteArrayInd",	/*0x3D*/
    "StoreIntoByteArray",	/*0x3E*/
    "IndexByteArrayInd",	/*0x3F*/
    "Between",			/*0x40*/
    "Not",			/*0x41*/
    "IntegerEquals",		/*0x42*/
    "StoreRetVal",		/*0x43*/
    "RetValReturn",		/*0x44*/
    NULL
};
