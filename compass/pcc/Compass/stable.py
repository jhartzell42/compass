class SelfEntry:
    def storeCode(self,block): ridiculous
    def loadCode(self,block):
	block.add_instruction('Self')
class InstanceVariableEntry:
    def __init__(self,num):
	self.num = num
    def getNumber(self):
	return self.num
    def storeCode(self,block):
	block.add_instruction('SStoreInst '+str(self.num))
    def loadCode(self,block):
	block.add_instruction('SInst '+str(self.num))

class ArgumentEntry:
    def __init__(self,num):
	self.num = num
    def storeCode(self,block):
	block.add_instruction('StoreArg '+str(self.num))
    def loadCode(self,block): 
	block.add_instruction('Arg '+str(self.num))

class TemporaryEntry:
    def __init__(self,block):
	self.num = block.incrementTemporaries()
    def storeCode(self,block):
	block.add_instruction('StoreTemp %i'%(self.num))
    def loadCode(self,block):
	block.add_instruction('Temp %i'%(self.num))
    def getNumber(self):
	return self.num
