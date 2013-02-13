from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression
from Compass.Bytecode.mtable import MethodTable
from Compass.Bytecode.globals import Global
from Compass.Bytecode.object import BytecodeObject
from Compass.Bytecode.block import Block
from Compass.exceptions import SyntaxError,SemanticError
from Compass.stable import InstanceVariableEntry
from .method import PseudoMethod,Selector
from .method import generateTrueMethod,generateFalseMethod

nameList = {}
class ClassDecl(ASTNode):
    def checkNameUniqueness(self):
	global nameList
	if self.name in nameList:
	    raise Exception('double class: %s'%(self.name))
	else:
	    nameList[self.name] = self
    def tryParse(self):
	self.parent = None
	self.exceptionHandlingBlocks = []
	self.exception = False
	self.mixins = []
	publicToken = self.tokenizer.check('public')
	privateToken = self.tokenizer.check('private')
	classToken = self.tokenizer.check('class')
	if not classToken:
	    classToken = self.tokenizer.check('exception')
	    if classToken:
		self.exception = True
	if classToken:
	    self.singleton = False
	else:
	    singletonToken = self.tokenizer.check('singleton')
	    if not singletonToken:
		if privateToken:
		    self.tokenizer.push(privateToken)
		if publicToken:
		    self.tokenizer.push(publicToken)
		return False
	    else:
		self.singleton = True
	sigilToken = self.tokenizer.check('$')
	if not sigilToken:
	    raise SyntaxError('sigil sigil error')
	idToken = self.tokenizer.nextToken()
	if idToken.type() != TokenTypes.identifier:
	    raise SyntaxError('Token type non-id -id '+idToken.errForm())
	self.name = idToken.str
	self.checkNameUniqueness()
	if self.tokenizer.check('('):
	    if not self.tokenizer.check('$'):
		raise SyntaxError('no sigil in class decl')
	    self.parent = self.tokenizer.nextToken().str
	    if self.parent == self.name and self.name != 'Object':
		raise SemanticError('Only $Object may inherit from itself.')
	    if not self.tokenizer.check(')'):
		raise SyntaxError('no close paren in class decl')
	else:
	    if self.exception:
		self.parent = 'Exception'
	    else:
		self.parent = 'Object'
	self.ivars = []
	if self.exception:
	    self.selector = tryParse(SelectorLiteral,self.tokenizer)
	while not self.tokenizer.check(';'):
	    if not self.tokenizer.check('!'):
		raise SyntaxError('; or !')
	    self.ivars += [self.tokenizer.nextToken().str]
	    if not self.tokenizer.check(','):
		self.tokenizer.require(';')
		break
	self.methodTable = None
	return True
    def addClassMethod(self,symbolTable):
	blk = Block()
	blk.add_instruction('Global $%s' % self.name)
	blk.add_instruction('Return')
	if not self.exceptionHandlingBlocks:
	    self.exceptionHandlingBlocks = []
	self.exceptionHandlingBlocks += [blk]

	class FakeSelector:
	    def __init__(self,name):
		self.messageName = name
	selector = FakeSelector('class')
	method = PseudoMethod(blk.getName(),selector)
	self.addMethod(symbolTable,selector,method) # XXX ???
	self.exceptionHandlingBlocks += [blk]
    def __str__(self):
	if self.singleton:
	    res = 'singleton $'
	else:
	    res = 'class $'
	res += self.name
	res += ' ($'
	res += self.parent
	res += ') '
	for ivar in self.ivars:
	    res +='!'
	    res += ivar
	    res += ','
	res += ";\n"
	return res
    def getMethodTable(self,symbolTable):
	if not self.methodTable:
	    if self.parent != self.name:
		parent = symbolTable[self.parent]
		self.methodTable =\
			parent.getMethodTable(symbolTable).child(self.name)
		self.classMethodTable =\
			parent.classMethodTable.child(self.name+"_class")
	    else:
		self.methodTable = MethodTable(self.name)
		self.classMethodTable = self.methodTable.child(self.name+\
			"_class")
	    for mixinName in self.mixins:
		mixin = symbolTable[mixinName]
		self.methodTable.addParent(mixin.getMethodTable(symbolTable))
		self.classMethodTable.addParent(mixin.classMethodTable)
	return self.methodTable
    def extractSymbols(self):
	return {self.name: self}
    def defineExceptionMethod(self,symbolTable):
	self.exception = False
	class FakeSelector:
	    def __init__(self,name):
		self.messageName = name
	testerSelector = FakeSelector('internal_is'+self.name)
	self.addMethod(symbolTable,testerSelector,
		    generateTrueMethod(testerSelector))
	symbolTable['Object'].addMethod(symbolTable,testerSelector,
		    generateFalseMethod(testerSelector))
	blk1 = Block()
	blk1.add_instruction('BlockArg 0')
	blk1.add_instruction("MCall 0 'internal_is%s'"%(self.name))
	blk2 = Block()
	blk2.add_instruction('Block #%s'%(blk1.getName()))
	blk2.add_instruction('Arg 0')
	blk2.add_instruction('Self')
	blk2.add_instruction("MCall 2 'onExceptionMatching:do:'")
	blk2.add_instruction("Return")
	self.exceptionHandlingBlocks = [blk1,blk2]
	symbolTable['Block'].addMethod(symbolTable,self.selector,
		PseudoMethod(blk2.getName(),self.selector))
    def addMixin(self,mixinName,symbolTable):
	self.mixins += [mixinName]
	if self.methodTable:
	    mixin = symbolTable[mixinName]
	    self.methodTable.addParent(mixin.getMethodTable(symbolTable))
	    self.classMethodTable.addParent(mixin.classMethodTable)
    def addMethod(self,symbolTable,selector,method):
	if self.exception:
	    self.defineExceptionMethod(symbolTable)
	self.getMethodTable(symbolTable)
	if method.isClass:
	    mt = self.classMethodTable
	elif method.isConstructor:
	    self.methodTable.addMethod(selector.messageName,
		    method.block.getName())
	    classMethodBlock = Block()
	    classMethodBlock.incrementTemporaries()
	    classMethodBlock.add_instruction('Self')
	    classMethodBlock.add_instruction("MCall 0 'basicNew'")
	    classMethodBlock.add_instruction("StoreTemp 0")
	    for i in range(0,method.getArgCount()):
		classMethodBlock.add_instruction("Arg %i"%(i))
	    classMethodBlock.add_instruction("Temp 0")
	    classMethodBlock.add_instruction("MCall %i '%s'"%\
		    (method.getArgCount(),selector.messageName))
	    classMethodBlock.add_instruction("Temp 0")
	    #classMethodBlock.add_instruction("MCall 0 'secure'")
	    classMethodBlock.add_instruction("Return")
	    self.exceptionHandlingBlocks += [classMethodBlock]
	    self.classMethodTable.addMethod(selector.messageName,
		    classMethodBlock.getName())
	    return
	else:
	    mt = self.methodTable
	if method.isAlias:
	    mt.addAlias(selector.messageName,method.aliasTarget.messageName)
	else:
	    mt.addMethod(selector.messageName,method.block.getName())
    def resolveMixins(self,stable):
	for mixinName in self.mixins:
	    # TODO mixin hierarchy?
	    methods = stable[self.mixinName].getMethods(stable)
	    for method in methods:
		(selector,method) = method
		#print 'MIXIN DEBUG', cls, selector, method
		# TODO conditional add
		cls.addMethod(stable,selector,method)
    def generateCode(self,stable):
	self.getMethodTable(stable) #ensure method tables initialized
	self.addClassMethod(stable)
	#self.resolveMixins(stable)
	gvar = Global()  # class/singleton object reference
	gvar.setName(self.name) # our name
	obj = BytecodeObject(self.name) # class/singleton object
	gvar.setValue(obj) # which the reference points to
	if self.singleton:
	    obj.setMTable(self.methodTable) #singletons get object mtable
	    return [self.methodTable,obj,gvar]+self.exceptionHandlingBlocks
	else:
	    obj.setMTable(self.classMethodTable) #non-singletons get factory one
	    block = Block(name = '_new_' + self.name) # new method
	    block.add_instruction("Int %i"%\
		    (self.getInstanceVariableCount(stable)))
	    block.add_instruction("Alloc %i"% \
		    self.getInstanceVariableCount(stable))
	    block.add_instruction("Dup")
	    block.add_instruction("ChMTable %%%s"%(self.methodTable.name))
	    block.add_instruction("Return")
	    self.classMethodTable.addMethod('basicNew',block.name)
	    return [self.methodTable,self.classMethodTable,block,obj,gvar]+\
		    self.exceptionHandlingBlocks
    def getParent(self,symbolTable):
	if self.name != self.parent:
	    return symbolTable[self.parent]
	else:
	    return None
    def getInstanceVariableCount(self,symbolTable):
	parent = self.getParent(symbolTable)
	if parent:
	    count = parent.getInstanceVariableCount(symbolTable)
	else:
	    count = 0
	return count + len(self.ivars)
    def generateInstanceSymbolTable(self,symbolTable):
	if self.singleton:
	    return {} #singletons haven't got any instance variables.
	parent = self.getParent(symbolTable)
	if parent:
	    ivarNum = parent.getInstanceVariableCount(symbolTable)
	else:
	    ivarNum = 0
	res = {}
	for ivar in self.ivars:
	    res[ivar] = InstanceVariableEntry(ivarNum)
	    ivarNum += 1
	return res
    def storeCode(self,block): ERROR
    def loadCode(self,block):
	block.add_instruction('Global $%s'%(self.name))
