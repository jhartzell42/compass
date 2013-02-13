from .bytecode import BytecodeComponent
from .bytecode import BytecodeReference

class ByteArray(BytecodeComponent):
    def sigil(self):
	return '@'
    def __init__(self,name):
	self.name = name
    def setValues(self,values):
	self.values = values
    def contents(self):
	i = 0
	res = "\nS"
	for value in self.values:
	    if i == 16:
		res += "\nS"
		i = 0 
	    res += str(value) + ' '
	    i += 1
	return res
