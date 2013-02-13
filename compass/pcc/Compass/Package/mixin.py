from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression
from Compass.Bytecode.mtable import MethodTable
from Compass.Bytecode.globals import Global
from Compass.Bytecode.object import BytecodeObject
from Compass.Bytecode.block import Block
from Compass.exceptions import SyntaxError
from Compass.stable import InstanceVariableEntry
from .method import generateTrueMethod,generateFalseMethod

class MixinReference(ASTNode):
    def tryParse(self):
	sigilToken = self.tokenizer.check('$')
	if not sigilToken:
	    return False
	classToken = self.tokenizer.nextToken()
	mixinToken = self.tokenizer.check('mixin')
	if not mixinToken:
	    self.tokenizer.push(classToken)
	    self.tokenizer.push(sigilToken)
	    return False
	self.cls = classToken.str
	if not self.tokenizer.check('$'):
	    raise SyntaxError('expecting $: '+\
		    self.tokenizer.nextToken().errForm())
	self.mixinName = self.tokenizer.nextToken().str
	if not self.tokenizer.check(';'):
	    raise SyntaxError('expecting ;: '+\
		    self.tokenizer.nextToken().errForm())
	return True
    def generateCode(self,stable):
	stable[self.cls].addMixin(self.mixinName,stable)
	return []

class MixinDecl(ASTNode):
    def tryParse(self):
	self.mixins = []
	self.methodTable = None
	self.classMethodTable = None
	self.mixinMethodTable = None
	publicToken = self.tokenizer.check('public')
	if not publicToken:
	    publicToken = self.tokenizer.check('private')
	mixinToken = self.tokenizer.check('mixin')
	if not mixinToken:
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	sigilToken = self.tokenizer.check('$')
	if not sigilToken:
	    raise SyntaxError('expected sigil: '+\
		    self.tokenizer.nextToken().errForm())
	idToken = self.tokenizer.nextToken()
	if idToken.type() != TokenTypes.identifier:
	    raise SyntaxError('expected: identifier: '+idToken.errForm())
	self.name = idToken.str
	self.selector = tryParse(SelectorLiteral,self.tokenizer)
	self.tokenizer.require(';')
	if self.selector:
	    self.methods = [(self.selector,generateTrueMethod(self.selector))]
	else:
	    self.methods = []
	return True
    def __str__(self):
	return 'mixin %s' % self.name
    def extractSymbols(self):
	return {self.name:self}
    def addMixin(self,mixinName,symbolTable):
	self.mixins += [mixinName]
	if self.methodTable:
	    mixin = symbolTable[mixinName]
	    self.methodTable.addParent(mixin.getMethodTable(symbolTable))
	    self.classMethodTable.addParent(mixin.classMethodTable)
    def addMethod(self,symbolTable,selector,method):
	#self.methods += [(selector,method)]
	self.getMethodTable(symbolTable)
	if method.isClass:
	    mt = self.classMethodTable
	elif method.isMixin:
	    mt = self.mixinMethodTable
	elif method.isConstructor:
	    raise Error('constructors on mixins not implemented')
	else:
	    mt = self.methodTable
	if method.isAlias:
	    mt.addAlias(selector.messageName,method.aliasTarget.messageName)
	else:
	    mt.addMethod(selector.messageName,method.block.getName())
    def getMethods(self,stable):
	if self.selector:
	    stable['Object'].addMethod(stable,self.selector,\
		    generateFalseMethod(self.selector))
	    self.selector = None
	return self.methods
    def getMethodTable(self,symbolTable):
	if not self.methodTable:
	    self.methodTable = MethodTable(self.name)
	    self.classMethodTable = self.methodTable.child(self.name+\
		    "_class")
	    for mixinName in self.mixins:
		mixin = symbolTable[mixinName]
		self.methodTable.addParent(mixin.getMethodTable(symbolTable))
		self.classMethodTable.addParent(mixin.classMethodTable)
	    self.mixinMethodTable = self.methodTable.child(self.name+\
		    "_mixin")
	    # TODO should have a $Mixin class, but use $Object for now.
	    self.mixinMethodTable.addParent(symbolTable['Object'].
		    getMethodTable(symbolTable))
	return self.methodTable
    def generateCode(self,stable):
	self.getMethodTable(stable) #ensure method tables initialized
	gvar = Global()  # mixin object reference
	gvar.setName(self.name) # our name
	obj = BytecodeObject(self.name) # class/singleton object
	gvar.setValue(obj) # which the reference points to
	obj.setMTable(self.mixinMethodTable) #mixin objects get mixin mtable
	return [self.mixinMethodTable,obj,gvar]
    def generateInstanceSymbolTable(self,symbolTable):
	return {}
    def loadCode(self,block):
	block.add_instruction('Global $%s'%(self.name))
