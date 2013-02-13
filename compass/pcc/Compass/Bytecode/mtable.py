from .bytecode import BytecodeComponent
currentMethodTable = 0
def generateDefaultName():
    global currentMethodTable
    currentMethodTable += 1
    return "table"+str(currentMethodTable)
nameList = {}
class MethodTable(BytecodeComponent):
    def sigil(self):
	return '%';
    def checkNameUniqueness(self):
	if self.name in nameList:
	    raise Exception('fubar')
	else:
	    nameList[self.name] = self
    def __init__(self,name=None,parent=None):
	self.methods = {}
	self.aliasedMethods = {}
	self.aliases = {}
	self.name = name or generateDefaultName()
	self.checkNameUniqueness()
	if parent is None:
	    self.parents = []
	else:
	    self.parents = [parent]
	self.methodsResolved = False
	self.aliasesResolved = False
	self.aliasesApplied = False
	self.resolved = False
    def child(self,name = None):
	res = MethodTable(name = name,parent = self)
	return res
    def addMethod(self,name,blockName):
	self.methods[name] = blockName
    def addAlias(self,name,target):
	self.aliases[name] = target
    def addParent(self,parent):
	self.parents += [parent]
    def applyAliases(self):
	if self.aliasesApplied:
	    return
	numChanges = 0
	for alias in self.aliases.keys():
	    target = self.aliases[alias]
	    if self.hasMethod(target) and \
		    (not self.hasMethod(alias) or
			    self.getMethods(alias) != self.getMethod(target)):
		self.aliasedMethods[alias] = self.getMethod(target)
		numChanges = numChanges + 1
	if numChanges > 0: # We are getting somewhere...
	    self.resolveAliases()
	self.aliasesApplied = True
    def resolveMethods(self):
	if self.methodsResolved:
	    return
	for parent in self.parents:
	    parent.resolveMethods()
	    for meth in parent.definedMethods():
		if meth not in self.methods.keys() and \
			meth not in self.aliases.keys():
		    self.methods[meth] = parent.getMethod(meth)
	self.methodsResolved = True
    def resolveAliases(self):
	if self.aliasesResolved:
	    return
	for parent in self.parents:
	    parent.resolveAliases()
	    for alias in parent.definedAliases():
		if alias not in self.aliases.keys() and \
			alias not in self.methods.keys():
		    self.aliases[alias] = parent.getAlias(alias)
	self.aliasesResolved = True
    def resolve(self):
	if self.resolved:
	    return
	self.resolveMethods()
	self.resolveAliases()
	self.applyAliases()
	self.resolved = True
    # Warning: Only call contents once you know all methods have been added
    def contents(self):
	self.resolve()
	res = "\n"
	res += 'default = #'+self.getDefault()+"\n"
	for method in self.methods.keys()+self.aliasedMethods.keys():
	    res += "'%s' = #%s\n"%(method,self.getMethod(method))
	return res
    def getDefault(self):
	return self.methods['messageNotUnderstood:']
    def definedMethods(self):
	return self.methods.keys()
    def isDefinedMethod(self,name):
	return name in self.methods.keys()
    def hasMethod(self,name):
	return name in self.methods.keys() or\
		name in self.aliasedMethods.keys()
    def getMethod(self,name):
	if name in self.methods:
	    return self.methods[name]
	elif name in self.aliasedMethods:
	    return self.aliasedMethods[name]
	else:
	    raise Error("No method named %s" % name)
    def definedAliases(self):
	return self.aliases.keys()
    def getAlias(self,name):
	return self.aliases[name]
