from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression

class CompassDecl(ASTNode):
    def tryParse(self):
	tokenA = self.tokenizer.check('needle')
	if not tokenA:
	    return False
	tokenB = self.tokenizer.check('2007')
	if not tokenB:
	    self.tokenizer.push(tokenA)
	    return False
	tokenC = self.tokenizer.check(';')
	if not tokenC:
	    self.tokenizer.push(tokenA)
	    self.tokenizer.push(tokenB)
	    return False
	return True

