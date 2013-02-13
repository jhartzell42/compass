from Compass.parser import ASTNode, tryParse
from Compass.Expression.literal import SelectorLiteral
from Compass.exceptions import SyntaxError

class MessageDecl(ASTNode):
    def tryParse(self):
	self.access = ''
	publicToken = self.tokenizer.check('public')
	if publicToken:
	    self.access = 'public'
	else:
	    publicToken = self.tokenizer.check('private')
	    if publicToken:
		self.access = 'private'
	self.selector = tryParse(SelectorLiteral, self.tokenizer)
	if not self.selector:
	    if publicToken:
		self.tokenizer.push(publicToken)
	    return False
	self.tokenizer.require(';')
	return True

    def generateCode(self,symbolTable):
	return []
