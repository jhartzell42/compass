from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpressionSimple
from Compass.Bytecode.block import Block
from Compass.Bytecode.globals import Global
from Compass.stable import SelfEntry,TemporaryEntry
from Compass.exceptions import SyntaxError

class GlobalDecl(ASTNode):
    def tryParse(self):
	publicToken = self.tokenizer.check('public')
	if not publicToken:
	    publicToken = self.tokenizer.check('private')
	if not self.tokenizer.check('global'):
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	self.tokenizer.require('$')
	idToken = self.tokenizer.nextToken()
	if idToken.type() != TokenTypes.identifier:
	    raise SyntaxError("expected identifier: "+idToken.errForm())
	self.name = idToken.str
	self.tokenizer.require('=')
	self.expr = parseExpressionSimple(self.tokenizer)
	if not self.expr:
	    raise SyntaxError('expression required for global')
	self.tokenizer.require(';')
	self.block = Block(innerBlock = False)
	return True
    def extractSymbols(self):
	return {self.name: self}
    def generateCode(self,symbolTable):
	test_global = Global()
	store_global = Global()
	test_global.setName('test'+self.name)
	store_global.setName('store'+self.name)

	secondary_block = Block(innerBlock = True)
	secondary_block.add_instruction('Global $store'+self.name)
	secondary_block.add_instruction('Return')

	blocks = []
	self.block.add_instruction('Block #'+secondary_block.getName())
	self.block.add_instruction('Global $test'+self.name)
	self.block.add_instruction("MCall 1 'and:'")
	self.block.add_instruction('Pop')
	self.expr.generateCode(symbolTable,
		{},
		{'_return': None,'@':None,'_blocks':blocks},
		None,
		self.block,
		self.block)
	self.block.add_instruction('Dup')
	self.block.add_instruction('StoreGlobal $store'+self.name)
	self.block.add_instruction('Global $true')
	self.block.add_instruction('StoreGlobal $test'+self.name)
	self.block.add_instruction("Return")
	return [self.block,secondary_block,store_global,test_global]+blocks
    def loadCode(self,block):
	block.add_instruction("Self")
	block.add_instruction("Call 0 #%s"%(self.block.getName()))
	return [self.block]
