from Compass.parser import ASTNode, tryParse
from Compass.Expression.literal import SelectorLiteral
from Compass.exceptions import SyntaxError

class AliasDecl(ASTNode):
    def tryParse(self):
	self.isConstructor = False
	self.access = ''
	publicToken = self.tokenizer.check('public')
	if publicToken:
	    self.access = 'public'
	else:
	    publicToken = self.tokenizer.check('private')
	    if publicToken:
		self.access = 'private'
	if not self.tokenizer.check('alias'):
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	self.isClass = False
	self.isFinal = True # all pure aliases are final
	self.isAlias = True # duh
	self.selector = tryParse(SelectorLiteral, self.tokenizer)
	if not self.selector:
	    raise SyntaxError('Expected selector; found: ' + \
		self.tokenizer.nextToken().errForm())
	self.aliasTarget = tryParse(SelectorLiteral, self.tokenizer)
	if not self.aliasTarget:
	    raise SyntaxError('Expected selector; found: ' + \
		self.tokenizer.nextToken().errForm())
	self.tokenizer.require(';')
	return True

    def generateCode(self,symbolTable):
	# Add alias to base of hierarchy.
	symbolTable['Object'].addMethod(symbolTable,self.selector,self)
	return []
