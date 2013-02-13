from .bytecode import BytecodeComponent

current_block_id = 0
def getNextName():
    global current_block_id
    current_block_id += 1
    return "block"+str(current_block_id)

class Block(BytecodeComponent):
    def __init__(self,name = None,innerBlock=False,nameSet = True):
	self.temporaries_count = 0
	if name:
	    global current_block_id
	    current_block_id += 1
	    if nameSet:
		name += "_"+str(current_block_id)
	else:
	    name = getNextName()
	self.name = name or getNextName()
	self.instructions = []
	self.innerBlock = innerBlock
    def sigil(self):
	return '#'
    def incrementTemporaries(self):
	self.temporaries_count += 1
	return self.temporaries_count - 1
    def contents(self):
	if self.innerBlock:
	    res = "\n"
	else:
	    res = "\nTemporaries " + str(self.temporaries_count) + "\n"
	for inst in self.instructions:
	    res+=inst+"\n"
	return res
    def add_instruction(self,inst):
	self.instructions += [inst]
