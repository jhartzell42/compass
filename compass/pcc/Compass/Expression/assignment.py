from .expression import Expression,parseExpression
from Compass.parser import tryParse,tryParseAll
from .keyword import parseKeywordMessageSend
from Compass.tokenizer import TokenTypes
from .identifier import SelfReference,SuperReference
from .mcall import MessageSend
from Compass.stable import TemporaryEntry
from Compass.exceptions import SyntaxError

def parseAssignmentExpression(tokenizer):
    res = tryParse(Assignment,tokenizer)
    if not res:
	res = parseReplacementExpression(tokenizer)
    return res

class ReplacementExpression(MessageSend): 
    def __init__(self,rcvr,slices,rightSide):
	self.receiver = rcvr
	self.slices = slices
	self.rightSide = rightSide
    def __str__(self):
	res = '('
	res += str(self.reciever)
	res += ':['
	for slice in self.slices:
	    res += str(slice)
	    res += '~'
	res = res[:-1]
	res += ']='
	res += str(self.rightSide)
	res += ')'
	return res
    def getArgs(self):
	res = self.slices
	if len(res) > 1:
	    while len(res)<3:
		res+=[None]
	return res+[self.rightSide]
    def messageName(self):
	if len(self.slices) > 1:
	    return ':[~~]='
	else:
	    return ':[]='

def parseReplacementExpression(tokenizer):
    leftSide = parseKeywordMessageSend(tokenizer)
    superToken = None
    if not leftSide:
	superToken = tokenizer.nextToken()
	if superToken and superToken.str == 'super':
	    leftSide = SuperReference()
	else:
	    if superToken:
		tokenizer.push(superToken)
	    superToken = None
    colon = tokenizer.check(':[')
    if not colon:
	if superToken:
	    tokenizer.push(superToken)
	return leftSide
    if not leftSide:
	leftSide = SelfReference()
    slices = []
    while True:
	slice = parseExpression(tokenizer)
	slices += [slice]
	if not tokenizer.check('~'):
	    if not tokenizer.check(']='):
		raise SyntaxError('wtf333?'+tokenizer.nextToken().errForm())
	    break
	if len(slices)>3:
	    raise SyntaxError('three slices only: '+\
		    tokenizer.nextToken().errForm())
    rightSide = parseAssignmentExpression(tokenizer)
    if not rightSide:
	raise SyntaxError(']= needs expr afterwards: ',
		tokenizer.nextToken().errForm())
    return ReplacementExpression(leftSide,slices,rightSide)

class Assignment(Expression):
    def tryParse(self):
	sigil = self.tokenizer.nextToken()
	ident = self.tokenizer.checkNoWhitespace()
	colon = self.tokenizer.check(':=')
	if not sigil or not ident or not colon or sigil.str not in ['@','!']\
		or ident.type() != TokenTypes.identifier:
		    if colon:
			self.tokenizer.push(colon)
		    if ident:
			self.tokenizer.push(ident)
		    if sigil:
			self.tokenizer.push(sigil)
		    return False
	self.ident = ident.str
	self.instance = (sigil.str == '!')
	self.expr = parseAssignmentExpression(self.tokenizer)
	if not self.expr:
	    raise SyntaxError(':= needs to be followed by an '+\
		    'expression of some sort!'+\
		    self.tokenizer.nextToken().errForm())
	return True
    def __str__(self):
	res = '('
	if self.instance:
	    res += '!'
	else:
	    res += '@'
	res += self.ident
	res += ':='
	res += str(self.expr)
	res += ')'
	return res
    def generateCode(self, gst, ist, lst, cref, blo, bli):
	self.expr.generateCode(gst, ist, lst, cref, blo, bli)
	bli.add_instruction('Dup')
	if self.instance:
	    ist[self.ident].storeCode(bli)
	else:
	    if self.ident not in lst:
		lst[self.ident] = TemporaryEntry(blo)
	    lst[self.ident].storeCode(bli)
