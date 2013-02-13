from .expression import Expression
from .postfix import parsePostfixExpression
from .identifier import SelfReference,SuperReference
from .mcall import MessageSend
BinaryOperators = {}
class BinaryOperation(MessageSend):
    def __init__(self,op,expr,expr2):
	self.op = op
	self.expr = expr
	self.expr2 = expr2
    def __str__(self):
	return '('+str(self.expr)+''+self.op+''+str(self.expr2)+')'
    def getArgs(self):
	return [self.expr2]
    def getReceiver(self):
	return self.expr
    def messageName(self):
	return '%s'%(self.op)

class TernaryOperation(MessageSend):
    def __init__(self,expr,expr2,expr3):
	self.args = [expr2,expr3]
	self.receiver = expr
    def messageName(self):
	return '?//'
    def __str__(self):
	res = '%s?%s//%s'%(self.receiver,self.args[0],self.args[1])
	return res

class BinaryOperator:
    def opStr(self): return self.opString
    def recognizes(self,token):
	return token and token.str == self.opStr()
    def __call__(self,expr = None,expr2 = None):
	return BinaryOperation(self.opStr(),expr,expr2)
    def __init__(self,opString,levelNum):
	self.opString = opString
	self.levelNum = levelNum
    def level(self): return self.levelNum
    def __str__(self):
	return self.opStr() + '('+str(self.level())+')'

def binaryOperator(opStr,level):
    if level not in BinaryOperators:
	BinaryOperators[level] = []
    BinaryOperators[level] += [BinaryOperator(opStr,level)]

binaryOperator('**',101)
binaryOperator('*',200)
binaryOperator('/',200)
binaryOperator('\\',200)
binaryOperator('%',200)
binaryOperator('+',300)
binaryOperator('-',300)
binaryOperator('<<',400)
binaryOperator('>>',400)
binaryOperator('>>>',400)
binaryOperator('&',500)
binaryOperator('|',600)
binaryOperator('...',700)
binaryOperator('.',800)
binaryOperator('->',900)
binaryOperator('|->',900)
binaryOperator('><',900)
binaryOperator('<',1000)
binaryOperator('>',1000)
binaryOperator('<=',1000)
binaryOperator('>=',1000)
binaryOperator('=',1100)
binaryOperator('!=',1100)
binaryOperator('==',1100)
binaryOperator('!==',1100)
binaryOperator('===',1100)
binaryOperator('!===',1100)
binaryOperator('<=>',1100)
binaryOperator('&&',1200)
binaryOperator('||',1300)
binaryOperator('<--',1400)
binaryOperator('=>',1500)

def parseBinaryExpression(tokenizer):
    return parseExpressionLevel(10000,tokenizer)

def parseExpressionLevel(a,tokenizer):
    while a not in BinaryOperators and a != 1350:
	a -= 1
	if a == -1:
	    return parsePostfixExpression(tokenizer)
    if a == 1350: # #?://: :-)
	expr = parseExpressionLevel(a-1,tokenizer)
	if not expr:
	    superToken = tokenizer.nextToken()
	    if superToken and superToken.str == 'super':
		expr = super = SuperReference()
	    else:
		expr = super = None
		if(superToken):
		    tokenizer.push(superToken)
	super = None
	if tokenizer.check('?'):
	    if expr == None:
		expr = SelfReference()
	    from expression import parseExpression
	    expr2 = parseExpression(tokenizer)
	    if not expr2 or not tokenizer.check('//'):
		raise SyntaxError('? req. //',tokenizer.nextToken().errForm())
	    expr3 = parseExpressionLevel(a,tokenizer)
	    if not expr3:
		raise SyntaxError("// goin' nowhere"+
			tokenizer.nextToken().errForm())
	    return TernaryOperation(expr,expr2,expr3)
	if expr and expr == super:
	    tokenizer.push(superToken)
	return expr
    if a%2 == 0: #code for left-associative
	expr = parseExpressionLevel(a-1,tokenizer)
	if not expr:
	    superToken = tokenizer.nextToken()
	    if superToken and superToken.str == 'super':
		expr = super = SuperReference()
	    else:
		if superToken:
		    tokenizer.push(superToken)
		super = None
	else:
	    super = None
	while True:
	    token = tokenizer.nextToken()
	    if not token:
		break
	    for op in BinaryOperators[a]:
		if op.recognizes(token):
		    if not expr:
			expr = SelfReference()
		    expr2 = parseExpressionLevel(a-1,tokenizer)
		    if not expr2:
			raise SyntaxError('require expr after op: '+\
				token.errForm())
		    expr = op(expr,expr2)
		    token = None
		    break
	    if token:
		tokenizer.push(token)
		break
	if expr and super == expr:
	    tokenizer.push(superToken)
	    return None
	else:
	    return expr
    else: #right-associative
	expr = parseExpressionLevel(a-1,tokenizer)
	if expr == None:
	    superToken = tokenizer.nextToken()
	    if superToken and superToken.str == 'super':
		super = expr = SuperReference()
	    else:
		super = None
		if superToken:
		    tokenizer.push(superToken)
	else:
	    super = None
	token = tokenizer.nextToken()
	for op in BinaryOperators[a]:
	    if op.recognizes(token):
		if expr == None:
		    expr = SelfReference()
		expr2 = parseExpressionLevel(a,tokenizer)
		return op(expr,expr2)
	if token:
	    tokenizer.push(token)
	if expr and expr == super:
	    tokenizer.push(superToken)
	    return None
	return expr
