from __future__ import with_statement
from Compass.tokenizer import Tokenizer,TokenTypes
from Compass.parser import tryParseAll,tryParse
from .expression import Expression,parseExpression
from Compass.package import PackageSpec
from .identifier import SelfReference
from sys import argv
from re import compile
from .mcall import MessageSend
from Compass.Bytecode.block import Block
from Compass.stable import TemporaryEntry
from Compass.Bytecode.byteArray import ByteArray
from Compass.Bytecode.object import BytecodeObject
from Compass.Bytecode.globals import Global
class Literal(Expression):
    pass

counter = 0
class StringLiteral(Expression):
    def tryParse(self):
	next = self.tokenizer.checkRegex(compile("^'.*$"))
	if not next:
	    return False
	self.value = next.str[1:]
	return True
    def __str__(self):
	return "(('"+self.value+"'))"
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	global counter
	counter += 1
	arrayObject = ByteArray("str_array"+str(counter))
	vals = []
	for chr in self.value:
	    vals += [ord(chr)]
	arrayObject.setValues(vals)
	stringObject = BytecodeObject("str_object"+str(counter))
	class MTable:
	    def __init__(self,name):
		self.name = name
	stringMTable = MTable("AsciiString")
	stringObject.setMTable(stringMTable)
	stringObject.addIntIVar(len(vals))
	stringObject.addIVar("@"+arrayObject.name)
	stringGlobal = Global()
	stringGlobal.setName("str_global"+str(counter))
	stringGlobal.setValue(stringObject)
	lSym['_blocks'].append(stringGlobal)
	lSym['_blocks'].append(arrayObject)
	lSym['_blocks'].append(stringObject)
	blocki.add_instruction("Global $str_global"+str(counter))

class NumberLiteral(Expression):
    def tryParse(self):
	next = self.tokenizer.nextToken()
	if not next:
	    return False
	sign = 1
	if next.str[-1] == 'n':
	    num = next.str[:-1]
	    sign = -1
	else:
	    num = next.str
	try:
	    self.value = int(num)*sign
	    return True
	except ValueError:
	    self.tokenizer.push(next)
	    return False
    def __str__(self):
	return str(self.value)
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	blocki.add_instruction("Int %s"%(self.value))

class SymbolLiteral(Expression):
    def tryParse(self):
	hashToken = self.tokenizer.check('#')
	if not hashToken:
	    return False
	stringToken = self.tokenizer.checkRegex(compile("^'"))
	if not stringToken:
	    self.tokenizer.push(hashToken)
	    return False
	self.value = stringToken.str[1:]
	return True
    def __str__(self):
	return "((#'"+self.value+"'))"

class BlockLiteral(Expression):
    def tryParseArgList(self,sigil):
	args = []
	id = None
	if sigil.str in ['@','!']:
	    id = self.tokenizer.checkNoWhitespace()
	    if not id or id.type() != TokenTypes.identifier:
		if not id:
		    self.tokenizer.push(id)
		self.tokenizer.push(sigil)
		return []
	token = self.tokenizer.nextToken()
	if not token or (token.str != ',' and token.str != '--'):
	    if token:
		self.tokenizer.push(token)
	    if id:
		self.tokenizer.push(id)
	    self.tokenizer.push(sigil)
	    return []
	if sigil.str in ['!','@']:
	    args += [sigil.str+id.str]
	else:
	    args += [sigil.str]
	if token.str == ',' and not self.tokenizer.check('--'):
	    sigil = self.tokenizer.nextToken()
	    rest = None
	    if sigil.str in ['!','@','_','@@']:
		rest = self.tryParseArgList(sigil)
	    if not rest:
		raise SyntaxError('args lists once begun must finish:'+\
			self.tokenizer.nextToken().errForm())
	    args += rest
	return args
    def tryParse(self):
	if not self.tokenizer.check('{'):
	    return False
	sigil = self.tokenizer.nextToken()
	if sigil.str in ['@','_','@@','!']:
	    self.args = self.tryParseArgList(sigil)
	else:
	    self.args = []
	    if sigil.str != '--':
		self.tokenizer.push(sigil)
	self.expr = parseExpression(self.tokenizer)
	if not self.expr:
	    self.expr = SelfReference()
	if not self.tokenizer.check('}'):
	    raise SyntaxError('need some } you know?'+\
		    self.tokenizer.nextToken().errForm())
	return True
    def __str__(self):
	res = '{'
	res += "\n"
	if self.args:
	    for arg in self.args:
		res += arg+','
	    res += '--'
	res += str(self.expr)
	res += "\n"
	res += '}'
	return res
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	# set up internal return stuff
	oldReturn = lSym['_return']
	lSym['_return'] = None
	oldSelf = lSym['@']
	# set up new block
	block = Block(blocko.getName()+"_inner",True)
	# transfer arguments
	argNumber = 0
	start = blocko.temporaries_count
	for arg in self.args:
	    # implicit given
	    if arg[0:2] == '@@':
		lSym['@'] = TemporaryEntry(blocko)
	    # assignment
	    elif arg[0:1] == '@':
		lSym[arg[1:]] = TemporaryEntry(blocko)
	    # ignore
	    elif arg[0:1] == '_':
		TemporaryEntry(blocko)
	    argNumber += 1
	finish = blocko.temporaries_count - 1
	if finish>=start:
	    block.add_instruction("BlockArgs %i %i"%(start,finish))
	blocki.add_instruction("Block #%s"%(block.getName()))
	self.expr.generateCode(gSym,iSym,lSym,cref,blocko,block)
	if lSym['_return']==True:
	    block.add_instruction('RetValReturn')
	elif lSym['_return']:
	    lSym['_return'].loadCode(block)
	    block.add_instruction('Return')
	lSym['_blocks'].append(block)
	lSym['_return'] = oldReturn
	lSym['@'] = oldSelf
	
class SelectorLiteral(Expression):
    def tryParse(self):
	token = self.tokenizer.checkRegex(compile('^#'))
	if not token:
	    return False
	if token.str != '#':
	    self.messageName = token.str[1:]
	    self.package = '__operator'
	    self.operator = True
	    return True
	self.operator = False
	self.package = tryParse(PackageSpec,self.tokenizer)

	id = self.tokenizer.nextToken()
	#id is used to store tentative extensions of the scope of
	#the selector literal
	if id == None or id.type() != TokenTypes.identifier:
	    # without a package, it might still be a symbol
	    if self.package == None:
		self.tokenizer.push(id)
		self.tokenizer.push(token)
		return False
	    else: #otherwise, can't be much of anything
		raise SyntaxError("Expect id after pkg "+
			str(self.package)+" token "+id.errForm())
	# we are guaranteed success from here on out
	self.messageName = id.str
	# is it a keyword-message?
	if not self.tokenizer.checkNoWhitespace(':'):
	    # no, we're finished
	    self.argCount = 0
	    return True
	# yes, keyword message
	# TODO: ugly code! ick! does it even work?
	self.messageName += ':'
	self.argCount = 1
	id = self.tokenizer.nextToken()
	if (not id) or id.type() != TokenTypes.identifier:
	    self.tokenizer.push(id)
	    return True
	while self.tokenizer.checkNoWhitespace(':'):
	    self.messageName += id.str + ':'
	    self.argCount += 1
	    id = self.tokenizer.checkNoWhitespace()
	    if (not id) or id.type() != TokenTypes.identifier:
		break
	if id:
	    self.tokenizer.push(id)
	return True
    def __str__(self):
	if self.package:
	    return "((#"+str(self.package)+self.messageName+"))"
	else:
	    return "((#"+self.messageName+"))"
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	block = Block(blocko.getName()+"_inner",True)
	blocki.add_instruction("Block #%s"%(block.getName()))
	if self.operator:
	    if self.messageName == '?//' or self.messageName == ':[]=':
		self.argCount = 2
	    elif self.messageName == '[~~]':
		self.argCount = 3
	    elif self.messageName == ':[~~]=':
		self.argCount = 4
	    else:
		self.argCount = 1
	for i in range(0,self.argCount):
	    block.add_instruction('BlockArg %i'%(i+1))
	block.add_instruction('BlockArg 0')
	if self.messageName == "+":
	    block.add_instruction("Add 1 '+'")
	else:
	    block.add_instruction("MCall %i '%s'"%\
		    (self.argCount,self.messageName))

	lSym['_blocks'].append(block)
