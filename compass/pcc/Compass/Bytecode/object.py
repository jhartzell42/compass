from .bytecode import BytecodeComponent
from .bytecode import BytecodeReference

class BytecodeObject(BytecodeComponent):
    def sigil(self):
	return '@'
    def __init__(self,name):
	self.mtable = None
	self.ivars = []
	self.name = name
    def setMTable(self,mtable):
	self.mtable = mtable
    def addIVar(self,ref):
	self.ivars+=["ref "+str(ref)+" 0 rw\n"]
    def addIntIVar(self,intVar):
	self.ivars+=["int "+str(intVar)+"\n"]
    def contents(self):
	res = "\n"
	res += "mtable = %%%s\n"%(self.mtable.name)
	for ivar in self.ivars:
	    res += ivar
	return res
