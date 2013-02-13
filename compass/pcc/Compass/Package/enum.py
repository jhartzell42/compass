from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression
from Compass.Bytecode.mtable import MethodTable
from Compass.Bytecode.globals import Global
from Compass.Bytecode.object import BytecodeObject
from Compass.Bytecode.block import Block
from .classes import ClassDecl
from Compass.exceptions import SyntaxError
from Compass.stable import InstanceVariableEntry
from .method import Selector,PseudoMethod

class EnumDecl(ASTNode):
    def tryParse(self):
	publicToken = self.tokenizer.check('public')
	if not publicToken:
	    publicToken = self.tokenizer.check('private')
	if not self.tokenizer.check('enum'):
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	self.tokenizer.require('$')
	idToken = self.tokenizer.nextToken()
	if idToken.type() != TokenTypes.identifier:
	    raise SyntaxError("expected identifier: "+idToken.errForm())
	self.name = idToken.str
	if self.tokenizer.check('('):
	    self.tokenizer.require('$')
	    self.parent = self.tokenizer.nextToken().str
	    self.tokenizer.require(')')
	else:
	    self.parent = 'Object'
	values = []
	selector = ''
	while not self.tokenizer.check(';'):
	    selectorPart = self.tokenizer.nextToken()
	    if not self.tokenizer.checkNoWhitespace(':'):
		raise SyntaxError("enum selector not keyword-y enough: "+\
			self.tokenizer.nextToken().errForm())
	    selector += selectorPart.str + ':'
	    self.tokenizer.require('(')
	    self.tokenizer.require('$')
	    valueToken = self.tokenizer.nextToken()
	    if valueToken.type() != TokenTypes.identifier:
		raise SyntaxError("value id's must be id's: "+\
			self.tokenizer.nextToken().errForm())
	    values += [valueToken.str]
	    self.tokenizer.require(')')
	self.values = values
	self.selector = selector
	return True
    def extractClasses(self):
	enumClass = ClassDecl()
	enumClass.parent = self.parent
	enumClass.exceptionHandlingBlocks = []
	enumClass.exception = False
	enumClass.singleton = False
	enumClass.name = self.name
	enumClass.ivars = []
	enumClass.mixins = []
	enumClass.methodTable = None
	valueClasses = []
	for value in self.values:
	    valueClass = ClassDecl()
	    valueClass.parent = self.name
	    valueClass.exceptionHandlingBlocks = []
	    valueClass.exception = False
	    valueClass.singleton = True
	    valueClass.name = value
	    valueClass.ivars = []
	    valueClass.mixins = []
	    valueClass.methodTable = None
	    valueClasses += [valueClass]
	return [enumClass]+valueClasses
    def generateCode(self,symbolTable):
	argNumber = 0
	blocks = []
	selector = Selector(self.selector,False)
	for value in self.values:
	    block = Block()
	    block.add_instruction('Arg %i'%(argNumber))
	    block.add_instruction("MCall 0 '()'")
	    block.add_instruction("Return")
	    symbolTable[value].addMethod(symbolTable,
		    selector,
		    PseudoMethod(block.getName(),selector))
	    blocks += [block]
	    argNumber = argNumber + 1
	return blocks
