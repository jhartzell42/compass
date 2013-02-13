from .expression import Expression
from .assignment import parseAssignmentExpression
from .identifier import SelfReference
from .mcall import MessageSend
from Compass.stable import TemporaryEntry
class ReturnExpression(Expression):
    def tryParse(self):
	op = self.tokenizer.check('^')
	if not op:
	    return False
	self.expr = parseAssignmentExpression(self.tokenizer)
	if not self.expr:
	    self.expr = SelfReference()
	return True
    def __str__(self):
	return '(^'+str(self.expr)+')'
    def generateCode(self, gst, ist, lst, cref, blo, bli):
	lst["_return"] = True
	self.expr.generateCode(gst, ist, lst, cref, blo, bli)
	bli.add_instruction('Dup')
	bli.add_instruction('StoreRetVal')
