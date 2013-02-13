from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression
from Compass.Bytecode.block import Block
from Compass.stable import SelfEntry,TemporaryEntry,ArgumentEntry
from Compass.exceptions import SyntaxError

class Selector:
    def __init__(self,messageName,operator=False):
	self.operator = operator
	self.messageName = messageName
    def __str__(self):
	return '#%s' % self.messageName

class PseudoMethod:
    def __init__(self,blockname,selector):
	class PseudoBlock:
	    def getName(self):
		return self.name
	self.block = PseudoBlock()
	self.block.name = blockname
	self.selector = selector
	self.isClass = False
	self.isMixin = False
	self.isAlias = False
	self.isConstructor = False

def generateTrueMethod(selector):
    return PseudoMethod("true",selector)

def generateFalseMethod(selector):
    return PseudoMethod("false",selector)

class MethodDecl(ASTNode):
    def tryParse(self):
	self.access = ''
	publicToken = self.tokenizer.check('public')
	if publicToken:
	    self.access = 'public'
	else:
	    publicToken = self.tokenizer.check('private')
	    if publicToken:
		self.access = 'private'
	dollarSign = self.tokenizer.check('$')
	if not dollarSign:
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	classToken = self.tokenizer.nextToken()
	self.cls = classToken.str
	self.isClass = self.tokenizer.check('class')
	self.isMixin = False # TODO mixin methods
	mixinToken = self.tokenizer.check('mixin')
	self.expr = None
	if mixinToken:
	    self.tokenizer.push(mixinToken)
	    self.tokenizer.push(classToken)
	    self.tokenizer.push(dollarSign)
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	# TODO constructor code generation
	self.isConstructor = self.tokenizer.check('constructor')
	self.isAbstract = self.tokenizer.check('abstract')
	self.isUndefine = self.tokenizer.check('undefine')
	if not self.isAbstract:
	    self.isAbstract = self.tokenizer.check('optional')
	self.isFinal = self.tokenizer.check('final')
	self.isAlias = self.tokenizer.check('alias')
	self.special = False
	self.selector = tryParse(SelectorLiteral, self.tokenizer)
	if self.selector:
	    if self.isAlias:
		self.aliasTarget = tryParse(SelectorLiteral, self.tokenizer)
		if not self.aliasTarget:
		    raise SyntaxError('Expected selector; found: ' + \
			    self.tokenizer.nextToken().errForm())
		self.tokenizer.require(';')
		return True
	    if self.isAbstract:
		self.tokenizer.require(';')
		return True
	    else:
		self.parseArgsWithSelector()
	else:
	    self.parseInterwoven()
	if self.special:
	    return True
	else:
	    self.parseSecureLevelDecl()
	    return self.parseMethodCode()
    def parseArgument(self):
	arg = self.tryParseArgument()
	if not arg:
	    raise SyntaxError("argument expected: "+\
		    self.tokenizer.nextToken().errForm())
	return arg
    def tryParseArgument(self):
	if self.tokenizer.check('@'):
	    id = self.tokenizer.nextToken()
	    if id.type() != TokenTypes.identifier:
		raise SyntaxError("not id: "+id.errForm())
	    return "@"+id.str
	elif self.tokenizer.check('_'):
	    return "_"
	elif self.tokenizer.check('!'):
	    id = self.tokenizer.nextToken()
	    if id.type() != TokenTypes.identifier:
		raise SyntaxError("not id: "+id.errForm())
	    return "!"+id.str
	else:
	    return None
    def getArgCount(self):
	return len(self.args)
    def parseInterwoven(self):
	nextToken = self.tokenizer.nextToken()
	if nextToken.type() == TokenTypes.operator:
	    if nextToken.str not in ['?','[',':[', '(']:
		self.args = [self.parseArgument()]
		self.selector = Selector(nextToken.str,True)
		return
	    elif nextToken.str == '?':
		self.selector = Selector('?//',True)
		self.args = [self.parseArgument()]
		self.tokenizer.require('//')
		self.args += [self.parseArgument()]
		return 
	    elif nextToken.str == ':[':
		self.args = [self.parseArgument()]
		if self.tokenizer.check('~'):
		    self.selector = Selector(':[~~]=',True)
		    self.args += [self.parseArgument()]
		    self.tokenizer.require('~')
		    self.args += [self.parseArgument()]
		else:
		    self.selector = Selector(':[]=',True)
		self.tokenizer.require(']=')
		self.args += [self.parseArgument()]
		return
	    elif nextToken.str == '[':
		self.args = [self.parseArgument()]
		if self.tokenizer.check('~'):
		    self.selector = Selector('[~~]',True)
		    self.args += [self.parseArgument()]
		    self.tokenizer.require('~')
		    self.args += [self.parseArgument()]
		else:
		    self.selector = Selector('[]',True)
		self.tokenizer.require(']')
		return
	    elif nextToken.str == '(':
		self.args = [self.parseArgument()]
		# TODO (for C3 probably) check if this is a list and match
		self.selector = Selector('()',True)
		self.tokenizer.require(')')
		return
	elif nextToken.type() == TokenTypes.identifier:
	    #so it's either a unary message-send or a keyworder.
	    colon = self.tokenizer.checkNoWhitespace(':')
	    if colon:
		self.args = [self.parseArgument()]
		name = nextToken.str + ':'
		while True:
		    nextToken = self.tokenizer.nextToken()
		    if nextToken.type() != TokenTypes.identifier:
			self.tokenizer.push(nextToken)
			break
		    self.tokenizer.require(':')
		    self.args += [self.parseArgument()]
		    name += nextToken.str + ':'
		self.selector = Selector(name,False)
	    else:
		self.args = []
		self.selector = Selector(nextToken.str,False)
	    return

    def parseArgsWithSelector(self):
	self.args = []
	while not self.tokenizer.peekCheck('{') and \
		not self.tokenizer.peekCheck(';') and \
		not self.tokenizer.peekCheck('('):
	    if not self.tokenizer.check('@'):
		if self.tokenizer.check('#'):
		    class Foo: pass
		    self.block = Foo()
		    name = self.tokenizer.nextToken().str
		    self.block.name = name
		    self.block.getName = lambda: name
		    self.special = True
		    self.name = self.block.name
		    self.tokenizer.check(';')
		    return True
		else:
		    raise SyntaxError('@ or {: '+
				    self.tokenizer.nextToken().errForm())
	    self.args += ['@'+self.tokenizer.nextToken().str]
	    if not self.tokenizer.check(','):
		break
    def parseSecureLevelDecl(self):
	# TODO check these values and store them in a cleaner way.
	# TODO Actually use securelevels.
	if not self.tokenizer.check('('):
	    return # no securelevel decl
	self.securelevel = self.tokenizer.nextToken()
	self.tokenizer.require(',')
	self.writability = self.tokenizer.nextToken()
	self.tokenizer.require(')')
    def parseMethodCode(self):
	if self.tokenizer.check(';'):
	    self.expr = None
	else:
	    self.tokenizer.require('{')
	    self.expr = parseExpression(self.tokenizer)
	    self.tokenizer.require('}')
	if self.selector.operator:
	    name = None
	else:
	    name = self.selector.messageName
	    name = name + "_" + self.cls
	    if self.isClass:
		name = name + "_class"
	self.block = Block(name = name,nameSet = False)
	self.name = self.block.getName()
	return True
    def __str__(self):
	res = '$'
	res += self.cls
	if self.isAlias:
	    res += ' alias '
	res += str(self.selector)
	res += ' '
	if self.isAlias:
	    res += str(self.aliasTarget)
	    res += ';'
	else:
	    for arg in self.args:
		res += '@'
		res += arg
		res += ', '
	    res += "{\n"
	    res += str(self.expr)
	    res += "\n}\n"
	return res
    def registerMethod(self,symbolTable):
	if not self.isAbstract: # TODO addAbstract() and check instead
	    symbolTable[self.cls].addMethod(symbolTable,self.selector,self)
    def generateCode(self,symbolTable):
	self.registerMethod(symbolTable)
	if self.isAlias or self.isAbstract or self.special:
	    return []
	cls = symbolTable[self.cls]
	itable = cls.generateInstanceSymbolTable(symbolTable)
	table = self.generateLocalSymbolTable(itable)
	table['_classMethod'] = self.isClass
	self.block.add_instruction('" '+self.selector.messageName)
	if self.expr:
	    self.expr.generateCode(symbolTable,
				   itable,
				   table,
				   cls,
				   self.block,
				   self.block)
	if table['_return'].__class__ == SelfEntry:
	    self.block.add_instruction("SReturn")
	elif table['_return']==True:
	    self.block.add_instruction("RetValReturn")
	elif table['_return']:
	    table['_return'].loadCode(self.block)
	    self.block.add_instruction("Return")
	return [self.block]+table['_blocks']
    def generateLocalSymbolTable(self,itable):
	res = {'@':SelfEntry(),
		'_return':SelfEntry(),
		'_blocks':[]}
	argNum = 0
	if self.selector.messageName == '()' and len(self.args) == 1:
	    self.block.add_instruction("AllocFromArgs")
	    self.block.add_instruction("Dup")
	    self.block.add_instruction("ChMTable %PrimitiveArray")
	    realArg = self.args[0]
	    if realArg[0] == '@':
		arg = realArg[1:]
		res[arg] = TemporaryEntry(self.block)
		self.block.add_instruction('StoreTemp %i'%\
			(res[arg].getNumber()))
	    elif realArg[0] == '_':
		self.block.add_instruction("Pop")
	    elif realArg[0] == '!':
		arg = realArg[1:]
		ivar = itable[arg]
		ivar.storeCode(self.block)
	else:
	    for realArg in self.args:
		if realArg[0] == '@':
		    arg = realArg[1:]
		    res[arg] = ArgumentEntry(argNum)
		elif realArg[0] == '_':
		    pass
		elif realArg[0] == '!':
		    arg = realArg[1:]
		    ivar = itable[arg]
		    self.block.add_instruction('Arg %i'%(argNum))
		    ivar.storeCode(self.block)
		argNum += 1
	return res
