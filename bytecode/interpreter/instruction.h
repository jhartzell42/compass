#ifndef BYTECODE_INSTRUCTION_H
#define BYTECODE_INSTRUCTION_H

typedef char instruction_t;

struct activation_record;
typedef struct activation_record *
(*implementation)(struct activation_record *curr);

extern implementation implementations[];
#define MAX_INSTRUCTION 0x35
#endif /* BYTECODE_INSTRUCTION_H */
