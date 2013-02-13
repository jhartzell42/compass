from __future__ import with_statement
from sys import argv
from Compass.tokenizer import MultiFileTokenizer
from Compass.SingleFile.file import CompassFile
from Compass.parser import tryParse

if __name__ == '__main__':
    mainClass = argv[1]
    tokenizer = MultiFileTokenizer(argv[2:])
    res = tryParse(CompassFile,tokenizer)
    t = tokenizer.nextToken()
    if t:
	print 'Error: tokens left over:',t.errForm()
	exit()
    print "#initialize"
    print "Global $BasicConsoleInputChannel"
    print "MCall 0 'new'"
    print "Global $BasicConsoleOutputChannel"
    print "MCall 0 'new'"
    print "Int 2"
    print "Global $BasicConsoleOutputChannel"
    print "MCall 1 'newWithFileDescriptor:'"
    print "Global $"+mainClass
    print "MCall 3 'newWithInput:andOutput:andError:'"
    print "MCall 0 'run'"
    print "Return"

    print "#true"
    print "Global $true"
    print "Return"

    print "#false"
    print "Global $false"
    print "Return"

    #TODO - actually implement this
    print "#SecurityReferenceCompare"
    print "Arg 0"
    print "Arg 1"
    print "Equals"
    print "Return"

    print "#PrimitiveArrayCreate"
    print "Arg 0"
    print "AllocInd"
    print "Dup"
    print "ChMTable %PrimitiveArray"
    print "Return"

    print "#MakeAsciiCharacter"
    print "Int 1"
    print "AllocByteArrayInd"
    print "Dup"
    print "Dup"
    print "ChMTable %AsciiCharacter"
    print "Dup"
    print "Arg 0"
    print "Swap"
    print "StoreIntoByteArray 0"
    print "Return"

    print "#ObjectIdentityHash"
    print "SReturn"

    print "@nil"
    print "mtable=%default"

    print "#SliceAsciiString"
    print "Arg 0"
    print "Arg 1"
    print "Arg 2"
    print "Arg 3"
    print "Self"
    print "Call 4 #ByteArraySliceInternal"
    print "Dup"
    print "Call 0 #GiveCorrectMTable"
    print "Return"

    print "#StringConcat"
    print "Arg 0"
    print "Self"
    print "Call 1 #ByteArrayConcatInternal"
    print "Dup"
    print "Call 0 #GiveCorrectMTable"
    print "Return"

    print "#GiveCorrectMTable"
    print "Block #CharacterMTable"
    print "SInst 0" #size
    print "Int 1" #character size
    print "Equals" #compare
    print "MCall 1 'and:'"
    print "Self"
    print "ChMTable %AsciiString"
    print "SReturn"

    print "#CharacterMTable"
    print "Self"
    print "ChMTable %AsciiCharacter"
    print "SReturn"

    print "#throw"
    print "Self"
    print "Throw"

    print "#ByteArraySlurp"
    print "Self"
    print "Call 0 #ByteArraySlurpInternal"
    print "Call 0 #GiveCorrectMTable"
    print "Return"

    for bytecodeComponent in res.generateCode():
	print bytecodeComponent
