from .expression import Expression
from .identifier import SuperReference
from .identifier import SelfReference
from Compass.stable import SelfEntry
class MessageSend(Expression):
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	args = self.getArgs()
	for arg in args:
	    if arg:
		arg.generateCode(gSym,iSym,lSym,cref,blocko,blocki)
	    else:
		blocki.add_instruction("Nil")
	receiver = self.getReceiver()
	if receiver.__class__ == SuperReference: 
	    parentClass = cref.getParent(gSym)
	    classMethod = lSym['_classMethod']
	    blockName = self.messageName()+"_"+parentClass.name
	    if classMethod:
		blockName += "_class"
	    selfReference = SelfReference()
	    selfReference.generateCode(gSym,iSym,lSym,cref,blocko,blocki)
	    blocki.add_instruction("Call %i #%s"%(len(args),blockName))
	else:
	    if self.messageName() == '+':
		string = "Add %i '%s'"
	    elif self.messageName() == '-':
		string = "Subtract %i '%s'"
	    elif self.messageName() == '*':
		string = "Multiply %i '%s'"
	    elif self.messageName() == '%':
		string = "Mod %i '%s'"
	    elif self.messageName() == '>':
		string = "Greater %i '%s'"
	    elif self.messageName() == '<':
		string = "Less %i '%s'"
	    elif self.messageName() == 'between:and:':
		string = "Between %i '%s'"
	    elif self.messageName() == '==':
		self.getReceiver().generateCode(gSym,iSym,lSym,cref,blocko,
			blocki)
		blocki.add_instruction("Equals")
		return
	    elif self.messageName() == 'self':
		# this basically does nothing :-)
		self.getReceiver().generateCode(gSym,iSym,lSym,cref,blocko,
			blocki)
		return
	    elif self.messageName() == 'not':
		string = "Not %i '%s'"
	    elif self.messageName() == '=':
		string = "IntegerEquals %i '%s'"
	    elif receiver.__class__ == SelfReference and \
		    lSym['@'].__class__ == SelfEntry:
		blocki.add_instruction("SMCall %i '%s'"%(len(args),
						     self.messageName()))
		return
	    else:
		string = "MCall %i '%s'"
	    self.getReceiver().generateCode(gSym,iSym,lSym,cref,blocko,blocki)
	    blocki.add_instruction(string%(len(args),self.messageName()))
    def getArgs(self): return self.args
    def getReceiver(self): return self.receiver
    def messageName(self): return self.mesgName
