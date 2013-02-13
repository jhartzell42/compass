class BytecodeComponent:
    def sigil(self): abstract
    def getName(self):
	return self.name
    def __str__(self):
	return self.output()
    def output(self):
	res = self.sigil()
	res += self.getName() 
	res += self.contents()
	return res
    def contents(self): abstract

def createIntegerReference(int):
    return BytecodeReference().setInt(int)
def createObjectReference(obj,sl,rw):
    res = BytecodeReference()
    res.setName(obj.name)
    res.setSecurity(sl)
    if rw:
	res.setWritable()
    else:
	res.setReadOnly()
    return res
class BytecodeReference:
    def __init__(self):
	self.isInt = False
    def setName(self,name):
	self.name = name
    def setInt(self,int):
	self.isInt = True
	self.int = int
    def setSecurity(self,sl):
	self.sl = sl
    def setWritable(self):
	self.rw = 'rw'
    def setReadOnly(self):
	self.rw = 'ro'
    def __str__(self):
	if self.isInt:
	    return "Int %i"%(self.int)
	else:
	    return "ref %s %i %s"%(self.name,self.sl,self.rw)
