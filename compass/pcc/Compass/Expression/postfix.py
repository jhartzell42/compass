from .primary import parsePrimaryExpression
from .identifier import SelfReference, SuperReference
from .list import parseExpressionList
from .expression import Expression, parseExpression
from Compass.parser import tryParse, tryParseAll
from Compass.package import PackageSpec
from Compass.tokenizer import TokenTypes
from Compass.stable import TemporaryEntry
from .mcall import MessageSend
from .list import List

class CallExpression(MessageSend):
    def __init__(self,receiver,args):
	self.receiver = receiver
	self.args = args
    def __str__(self):
	res = '('+str(self.receiver)
	res += '('
	for arg in self.args:
	    res += str(arg)
	    res += ','
	res += '))'
	return res
    def messageName(self): return '()'

class IndexExpression(MessageSend):
    def __init__(self,receiver,index):
	self.receiver = receiver
	self.index = index
    def __str__(self):
	return '('+str(self.receiver) + '[' + str(self.index) + '])'
    def getArgs(self): return [self.index]
    def messageName(self): return '[]'

class SliceExpression(MessageSend):
    def __init__(self,receiver,start,end,step=None):
	self.receiver = receiver
	self.start = start
	self.end = end
	self.step = step
    def __str__(self):
	res = '('+str(self.receiver) + '[' + str(self.start) + '~' + \
		str(self.end)
	if self.step:
	    res += '~' + str(self.step)
	res += '])'
	return res
    def getArgs(self):
	return [self.start,self.end,self.step]
    def messageName(self): return '[~~]'

class GivenExpression(Expression):
    def __init__(self,given,expr):
	self.given = given
	self.expr = expr
    def __str__(self):
	return '('+str(self.given) + '{' + str(self.expr) + '})'
    def generateCode(self, gst, ist, lst, cref, blo, bli):
	self.given.generateCode(gst, ist, lst, cref, blo, bli)
	temp = TemporaryEntry(blo)
	temp.storeCode(bli)
	oldTemp = lst['@']
	lst['@'] = temp
	self.expr.generateCode(gst, ist, lst, cref, blo, bli)
	lst['@'] = oldTemp

class UnaryMessageSend(MessageSend):
    def __init__(self,receiver,package,message):
	self.receiver = receiver
	self.package = package
	self.id = message
	self.args = []
    def __str__(self):
	res = '('+str(self.receiver)
	res += ' '
	if self.package:
	    res += str(self.package)
	res += self.id + ')'
	return res
    def messageName(self): return self.id
def parseRestOfSliceCall(tokenizer,receiverExpr,startIndex):
    expr = receiverExpr
    index = startIndex
    if tokenizer.check(']'):
	return IndexExpression(expr,index)
    if tokenizer.check('~'):
	index2 = parseExpression(tokenizer)
	if tokenizer.check(']'):
	    expr = SliceExpression(expr,index,index2)
	elif tokenizer.check('~'):
	    step = parseExpression(tokenizer)
	    if not tokenizer.check(']'):
		raise SyntaxError('need a ] eventually: '+
			    tokenizer.nextToken().errForm())
	    expr = SliceExpression(expr,index,index2,step)
	else:
	    raise SyntaxError('expected ] or ~: '+
		    tokenizer.nextToken().errForm())
    else:
	raise SyntaxError('expected ~ or ]: '+
		tokenizer.nextToken().errForm())
    return expr

def parsePostfixExpression(tokenizer):
    if tokenizer.check('['):
	expr = parseExpression(tokenizer)
	if expr:
	    if tokenizer.check(','):
		list = [expr]
		list += parseExpressionList(tokenizer)
		if not tokenizer.check(']'):
		    raise SyntaxError('close your brackets: '+\
			    tokenizer.nextToken().errForm())
		expr = List(list)
	    else:
		expr = parseRestOfSliceCall(tokenizer,
			SelfReference(),
			expr)
	else:
	    if tokenizer.check(']'):
		expr = List([])
	    else:
		raise SyntaxError('close your brackets: '+
			tokenizer.nextToken().errForm())
    elif tokenizer.check('('):
	expr = parseExpression(tokenizer)
	if expr:
	    if tokenizer.check(','):
		list = [expr]
		list += parseExpressionList(tokenizer)
		if not tokenizer.check(')'):
		    raise SyntaxError('close your parentheses: '+\
			    tokenizer.nextToken().errForm())
		expr = CallExpression(SelfReference(),list)
	    else:
		if not tokenizer.check(')'):
		    raise SyntaxError('not right paren: '+\
			    tokenizer.nextToken().errForm())
	    #expr has been stored into
	else:
	    if tokenizer.check(')'):
		expr = CallExpression(SelfReference(),[])
	    else:
		raise SyntaxError('paren-?'+tokenizer.nextToken().errForm())
    else:
	expr = parsePrimaryExpression(tokenizer)
    implicitSelf = None
    if not expr:
	if tokenizer.check('super'):
	    expr = SuperReference()
	else:
	    expr = SelfReference()
	    implicitSelf = expr
    # expr is the only variable we care about, and it is guaranteed to have
    # something in it

    # so now scoop up any postfixes
    oldExpr = None
    while oldExpr != expr:
	# expr refers to the expression so far
	oldExpr = expr
	if tokenizer.check('['):
	    index = parseExpression(tokenizer)
	    expr = parseRestOfSliceCall(tokenizer,expr,index)
	if tokenizer.check('('):
	    expressionList = parseExpressionList(tokenizer)
	    if not tokenizer.check(')'):
		raise SyntaxError('needed closing ): '+
			tokenizer.nextToken().errForm())
	    expr = CallExpression(expr,expressionList)
	if tokenizer.check('{'):
	    block = parseExpression(tokenizer)
	    expr = GivenExpression(expr,block)
	    if not tokenizer.check('}'):
		raise SyntaxError("expecting '}': "+\
			tokenizer.nextToken().errForm())
	package = tryParse(PackageSpec,tokenizer)
	unaryMsg = tokenizer.nextToken()
	if unaryMsg:
	    colon = tokenizer.checkNoWhitespace(':')
	    if not colon and unaryMsg.type() == TokenTypes.identifier:
		expr = UnaryMessageSend(expr,package,unaryMsg.str)
	    else:
		if colon:
		    tokenizer.push(colon)
		tokenizer.push(unaryMsg)
		if package:
		    package.pushBack(tokenizer)
	else:
	    if package:
		package.pushBack(tokenizer)
    if expr == implicitSelf:
	return None
    return expr
