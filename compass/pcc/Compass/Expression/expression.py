from __future__ import with_statement
from Compass.parser import ASTNode,tryParse,tryParseAll
from sys import argv
from Compass.tokenizer import Tokenizer

def parseExpressionSimple(tokenizer):
    from returns import ReturnExpression
    from assignment import parseAssignmentExpression
    res = tryParse(ReturnExpression,tokenizer)
    if not res:
	res = parseAssignmentExpression(tokenizer)
    if res:
	return res

class Expression(ASTNode):
    pass

class SemicolonExpression(Expression):
    def __init__(self,expr1,expr2):
	self.expr1 = expr1
	self.expr2 = expr2
    def __str__(self):
	return '('+str(self.expr1)+';'+str(self.expr2)+')'
    def generateCode(self, gst, ist, lst, cref, blo, bli):
	self.expr1.generateCode(gst, ist, lst, cref, blo, bli)
	bli.add_instruction('Pop')
	self.expr2.generateCode(gst, ist, lst, cref, blo, bli)

def parseExpression(tokenizer):
    expr = parseExpressionSimple(tokenizer)
    if tokenizer.check(';'):
	expr2 = parseExpression(tokenizer)
	if expr2:
	    expr = SemicolonExpression(expr,expr2)
    return expr
