#include "file.h"
#include "globals.h"
#include "Block.ih"
#include "Value.ih"
#include "debug.h"
#include "exec.ih"
#include "Object.ih"
#include "instructions.ih"
#include "builtins.ih"
#include "ByteArray.ih"

#include <iostream>

#include "Block.ch"
#include "file.ch"

int main(int argc, char **argv) {
    assert(argc == 2);
    load_file(argv[1]);
    std::cout << initial_block << std::endl;
    bytecode_run(new ActivationRecord(initial_block,
				      *bytecode_nil,
				      new MethodInvocation(0),
				      0,
				      false,
				      true,
				      0));
		 return 0;
}
