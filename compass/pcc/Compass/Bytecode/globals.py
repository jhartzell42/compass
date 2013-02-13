from .bytecode import BytecodeComponent

class Global(BytecodeComponent):
    def __init__(self):
	self.value = None
    def sigil(self):
	return '$'
    def setName(self,name):
	self.name = name
    def setValue(self,value):
	self.value = value
    def contents(self):
	if self.value:
	    return " = "+"ref "+self.value.sigil()+self.value.getName()+" 0 rw"
	else:
	    return " = false"
