from __future__ import with_statement
from .expression import parseExpression, Expression
from Compass.exceptions import SyntaxError
from .mcall import MessageSend
from Compass.stable import TemporaryEntry

def parseExpressionList(tokenizer):
    items = []
    while not items or tokenizer.check(','):
	expr = parseExpression(tokenizer)
	if not expr:
	    break
	items += [expr]
    return items

class List(Expression):
    def __init__(self,list,resizable=True):
	self.list = list
	self.resizable = resizable
    def __str__(self):
	res = '['
	for item in self.list:
	    res += str(item)+','
	res += ']'
	return res
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	if self.resizable:
	    blocki.add_instruction('Int %i'%(len(self.list)))
	    blocki.add_instruction('Global $ArrayList')
	    blocki.add_instruction("MCall 1 'newWithCapacity:'")
	else:
	    blocki.add_instruction('Alloc %i'%(len(self.list)+1))
	    blocki.add_instruction('Dup')
	    blocki.add_instruction('ChMTable %PrimitiveArray')
	temporary = TemporaryEntry(blocko)
	temporary.storeCode(blocki)
	if not self.resizable:
	    blocki.add_instruction('Int %i'%(len(self.list)))
	    blocki.add_instruction('Int 0')
	    temporary.loadCode(blocki)
	    blocki.add_instruction("StoreInstInd")
	    i = 0
	for expr in self.list:
	    expr.generateCode(gSym,iSym,lSym,cref,blocko,blocki)
	    if not self.resizable:
		blocki.add_instruction('Int %d' % (i+1))
	    temporary.loadCode(blocki)
	    if self.resizable:
		blocki.add_instruction("MCall 1 'addLast:'")
	    else:
		blocki.add_instruction("StoreInstInd")
		i += 1
	    if self.resizable:
		blocki.add_instruction("Pop")
	temporary.loadCode(blocki)
