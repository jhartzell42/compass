from __future__ import with_statement
from Compass.tokenizer import Tokenizer,TokenTypes
from Compass.parser import tryParseAll,tryParse
from .expression import Expression
from Compass.package import PackageSpec
from Compass.exceptions import SyntaxError
from Compass.stable import TemporaryEntry
from sys import argv
from re import compile

class VariableRef(Expression):
    def sigil(self): abstract
    def acceptsPackages(self):
	return False
    def checkNameTokenDefault(self,token):
	return token and token.type() == TokenTypes.identifier
    def checkNameToken(self,token):
	return self.checkNameTokenDefault(token)
    def tryParse(self):
	sigil = self.tokenizer.check(self.sigil())
	if not sigil:
	    return False
	if self.acceptsPackages():
	    self.package = tryParse(PackageSpec,self.tokenizer)
	    id = self.tokenizer.nextToken()
	else:
	    id = self.tokenizer.checkNoWhitespace()
	if not self.checkNameToken(id):
	    if not id:
		raise SyntaxError("nothing after sigil" + sigil.errForm())
	    raise SyntaxError("non-id after sigil: "+id.errForm())
	self.name = id.str
	return True
    def __str__(self):
	res = self.sigil()
	if self.acceptsPackages() and self.package:
	    res += str(self.package)
	res += self.name
	return res
	

class LocalVariableRef(VariableRef):
    def sigil(self):
	return '@'
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	if self.name not in lSym:
	    lSym[self.name] = TemporaryEntry(blocko)
	lSym[self.name].loadCode(blocki)

class InstanceVariableRef(VariableRef):
    def sigil(self):
	return '!'
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	iSym[self.name].loadCode(blocki)

class PackageVariableRef(VariableRef):
    def acceptsPackages(self):
	return True
    def checkNameToken(self,token):
	return self.checkNameTokenDefault(token) or token.str == '$'
    def sigil(self):
	return '$'
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	if self.name not in gSym:
	    print gSym
	gSym[self.name].loadCode(blocki)

class SelfReference(Expression):
    def tryParse(self):
	if self.tokenizer:
	    return self.tokenizer.check('@@')
    def __str__(self):
	return '@@'
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	lSym['@'].loadCode(blocki)

class SuperReference(Expression):
    def tryParse(self):
	return self.tokenizer.check('super')
    def __str__(self):
	return 'super'
    def generateCode(self,gSym,iSym,lSym,cref,blocko,blocki):
	raise Error("wtf, mate? super can't generate no code")
