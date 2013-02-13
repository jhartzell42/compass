from Compass.exceptions import SyntaxError

def CompassDecl(ASTNode):
    def tryParse(self):
	token = self.tokenizer.check('compass')
	if not token:
	    return False
	while token.str != ';':
	    token =  self.tokenizer.nextToken():
	    if not token:
		raise SyntaxError('compass decl. needs ;')
	return True
def PackageDecl(ASTNode):
    def tryParse(self):
	if not self.tokenizer.check('package'):
	    return False
	self.package = tryParse(PackageSpec, self.tokenizer)
	if not self.tokenizer.check(';'):
	    raise SyntaxError('package decl needs ;'+\
		    self.tokenizer.nextToken().errForm())
	return True
def ImportStatement(ASTNode):
    def tryParse(self):
	if not self.tokenizer.check('import'):
	    return False
	self.library = not not self.tokenizer.check('library')
	self.package = tryParse(PackageSpec, self.tokenizer)
	if not self.tokenizer.check(';'):
	    raise SyntaxError('import decl needs ;'+\
		    self.tokenizer.nextToken().errForm())
	return True
class PackageList(ASTNode):
    def tryParse(self):
	self.packages = []
	while true:
	    package = tryParse(PackageSpec, self.tokenizer)
	    if not package:
		break
	    self.packages += [package]
	    if not self.tokenizer.check(','):
		break
	return True
class TrustedDecl(ASTNode):
    def tryParse(self):
	if not self.tokenizer.check('trust'):
	    return False
	self.packages = tryParse(PackageList,self.tokenizer)
	if not self.tokenizer.check(';'):
	    raise SyntaxError('trusted...; <-!'+\
		    self.tokenizer.nextToken().errForm())
	return True
def File(ASTNode):
    def tryParse(self):
	decl = tryParse(CompassDecl,self.tokenizer)
	if not decl:
	    return False
	self.package = tryParse(PackageDecl,self.tokenizer)
	if not self.package:
	    raise SyntaxError('need package decl: '+\
		    self.tokenizer.nextToken().errForm())
	self.imports = []
	while True:
	    importStatement = tryParse(ImportDecl,self.tokenizer)
	    if not importStatement:
		break
	    self.imports += [importStatement]
	self.library = tryParse(LibraryDecl,self.tokenizer)
	self.trusted = tryParse(TrustDecl,self.tokenizer)
	while True:
	    self.add(parseFilePart(self.tokenizer))
