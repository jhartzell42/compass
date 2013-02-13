#include <stdlib.h>

#include "builtins.h"
#include "memory_region.h"

static const char *builtins[] = {
    NULL,
    "#SmallIntegerAdd",
    "#SmallIntegerSub",
    "#SmallIntegerMul",
    "#SmallIntegerDiv",
    "#SmallIntegerMod",
    "#SmallIntegerNegate",
    "#SmallIntegerLess",
    "#SmallIntegerGreater",
    "#BlockEvaluate",
    "#SimpleKeyboardIOReadInt",
    "#SimpleKeyboardIOReadChar",
    "#SimpleKeyboardIOPrintInt",
    "#SimpleKeyboardIOPrintChar",
    "#Abort",
    "#SimpleKeyboardIOPrintMethodTableName",
    "#BlockRepeat",
    "#BlockTry",
    "#IOPrintChar",
    "#IOReadChar",
    "#ReferenceCompare",
    "#PrimitiveArraySet",
    "#PrimitiveArrayGet",
    "#StringGet",
    "#ByteArraySet",
    "#ByteArraySliceInternal",
    "#StringCompare",
    "#EvaluateFirstArgument",
    "#EvaluateSecondArgument",
    "#ReturnTrue",
    "#ReturnFalse",
    "#ByteArrayConcatInternal",
    "#ByteArraySlurpInternal",
    NULL
};

void add_all_builtins() {
    int i;
    for(i = 1; builtins[i]; i++) {
	struct memory_region *mr = mr_new(builtins[i],2);
	mr->data[0] = i;
    }
}
