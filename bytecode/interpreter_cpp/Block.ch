#include "Block.ih"
const char *const Block::bytecodes[] = {
    "FINISH",       /*0x00*/
    "Alloc",        /*0x01*/
    "AllocInd",     /*0x02*/
    "Arg",          /*0x03*/
    "Block",        /*0x04*/
    "BlockArg",     /*0x05*/
    "Call",         /*0x06*/
    "ChMTable",     /*0x07*/
    "Dup",          /*0x08*/
    "Equals",       /*0x09*/
    "False",        /*0x0A*/
    "Global",       /*0x0B*/
    "Inst",         /*0x0C*/
    "InstInd",      /*0x0D*/
    "Int",          /*0x0E*/
    "IsRw",         /*0x0F*/
    "MCall",        /*0x10*/
    "Nil",          /*0x11*/
    "Pop",          /*0x12*/
    "RefEquals",    /*0x13*/
    "Return",       /*0x14*/
    "Ro",           /*0x15*/
    "Rw",           /*0x16*/
    "SChMTable",    /*0x17*/
    "Secure",       /*0x18*/
    "Self",         /*0x19*/
    "SInst",        /*0x1A*/
    "SInstInd",     /*0x1B*/
    "SIsRw",        /*0x1C*/
    "Sl",           /*0x1D*/
    "SMCall",       /*0x1E*/
    "SReturn",      /*0x1F*/
    "SRo",          /*0x20*/
    "SRw",          /*0x21*/
    "SSecure",      /*0x22*/
    "SSl",          /*0x23*/
    "StoreGlobal",  /*0x24*/
    "StoreInst",    /*0x25*/
    "StoreInstInd", /*0x26*/
    "StoreTemp",    /*0x27*/
    "SStoreInst",   /*0x28*/
    "SStoreInstInd",/*0x29*/
    "Swap",         /*0x2A*/
    "Temp",         /*0x2B*/
    "Temporaries",  /*0x2C*/
    "True",         /*0x2D*/
    "TSelf",        /*0x2E*/
    "TSMCall",      /*0x2F*/
    "AllocFromArgs",/*0x30*/
    "StoreArg",     /*0x31*/
    "StoreBlockArg",/*0x32*/
    "BlockEvaluate",/*0x33*/
    "Throw",        /*0x34*/
    "BlockArgs",    /*0x35*/
    "Add",          /*0x36*/
    "Subtract",     /*0x37*/
    "Multiply",     /*0x38*/
    "Divide",       /*0x39*/
    "Mod",          /*0x3A*/
    "Greater",      /*0x3B*/
    "Less",         /*0x3C*/
    "AllocByteArrayInd",        /*0x3D*/
    "StoreIntoByteArray",       /*0x3E*/
    "IndexByteArrayInd",        /*0x3F*/
    "Between",                  /*0x40*/
    "Not",                      /*0x41*/
    "IntegerEquals",            /*0x42*/
    "StoreRetVal",              /*0x43*/
    "RetValReturn",             /*0x44*/
};

const OperandType Block::operand_descriptions[][10] = {
    {NIL},			// FINISH
    { INT , NIL},		// Alloc
    {NIL},			// AllocInd
    { INT , NIL},		// Arg
    { BLOCK , NIL},		// Block
    { INT , NIL},		// BlockArg
    { INT, BLOCK , NIL},	// Call
    { METHOD_TABLE , NIL},	// ChMTable
    {NIL},			// Dup
    {NIL},			// Equals
    {NIL},			// False
    { GLOBAL , NIL},		// Global
    { INT , NIL},		// Inst
    { NIL},			// InstInd
    { INT, NIL },		// Int
    {NIL},			// IsRw
    { INT, SELECTOR , NIL},	// MCall
    {NIL},			// Nil
    {NIL},			// Pop
    {NIL},			// RefEquals
    {NIL},			// Return
    {NIL},			// Ro
    {NIL},			// Rw
    { METHOD_TABLE , NIL},	// SChMTable
    {INT,NIL},			// Secure
    {NIL},			// Self
    { INT , NIL},		// SInst
    {NIL},			// SInstInd
    {NIL},			// SIsRw
    {NIL},			// Sl
    { INT, SELECTOR , NIL},	// SMCall
    {NIL},			// SReturn
    {NIL},			// SRo
    {NIL},			// SRw
    {INT,NIL},			// SSecure
    {NIL},			// SSl
    { GLOBAL , NIL},		// StoreGlobal
    { INT , NIL},		// StoreInst
    {NIL},			// StoreInstInd
    { INT , NIL},		// StoreTemp
    { INT , NIL},		// SStoreInst
    {NIL},			// SStoreInstInd
    {NIL},			// Swap
    { INT , NIL},		// Temp
    { INT , NIL},		// Temporaries
    {NIL},			// True
    {NIL},			// TSelf
    { INT, SELECTOR , NIL},	// TSMCall
    {NIL},			// AllocFromArgs
    { INT , NIL},		// StoreArg
    { INT , NIL},		// StoreBlockArg
    { INT, SELECTOR, NIL},	// BlockEvaluate
    {NIL},			// Throw
    { INT, INT , NIL},	    	// BlockArgs
    { INT, SELECTOR, NIL},	// Add
    { INT, SELECTOR, NIL},	// Subtract
    { INT, SELECTOR, NIL},	// Multiply
    { INT, SELECTOR, NIL},	// Divide
    { INT, SELECTOR, NIL},	// Mod
    { INT, SELECTOR, NIL},	// Greater
    { INT, SELECTOR, NIL},	// Less
    {NIL},			// AllocByteArrayInd
    { INT , NIL},		// StoreIntoByteArray
    {NIL},			// IndexByteArrayInd
    { INT, SELECTOR, NIL},	// Between
    { INT, SELECTOR, NIL},	// Not
    { INT, SELECTOR, NIL},	// IntegerEquals
    {NIL},			// StoreRetVal
    {NIL},			// RetValReturn
};
