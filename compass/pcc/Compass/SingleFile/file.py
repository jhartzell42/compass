from Compass.parser import ASTNode, tryParse
from Compass.tokenizer import TokenTypes
from Compass.Expression.literal import SelectorLiteral
from Compass.Expression.expression import parseExpression
from Compass.Package.decl import CompassDecl
from Compass.Package.classes import ClassDecl
from Compass.Package.method import MethodDecl
from Compass.Package.globals import GlobalDecl
from Compass.Package.message import MessageDecl
from Compass.Package.enum import EnumDecl
from Compass.Package.alias import AliasDecl
from Compass.Package.mixin import MixinDecl,MixinReference

class CompassFile(ASTNode):
    def tryParse(self):
	if not tryParse(CompassDecl,self.tokenizer):
	    return False
	self.classDecl = []
	self.methodDecl = []
	self.aliasDecl = []
	self.mixinDecl = []
	self.globalDecl = []
	self.enumDecl = []
	while True:
	    newClass = tryParse(ClassDecl,self.tokenizer)
	    newMixin = tryParse(MixinDecl,self.tokenizer)
	    newMixinReference = tryParse(MixinReference,self.tokenizer)
	    newMethod = tryParse(MethodDecl,self.tokenizer)
	    newMessage = tryParse(MessageDecl,self.tokenizer)
	    newAlias = tryParse(AliasDecl,self.tokenizer)
	    compassDecl = tryParse(CompassDecl,self.tokenizer)
	    newGlobal = tryParse(GlobalDecl,self.tokenizer)
	    newEnum = tryParse(EnumDecl,self.tokenizer)
	    if not newClass and not newMethod and not compassDecl\
		    and not newMixin and not newMixinReference\
		    and not newAlias and not newMessage and not newGlobal\
		    and not newEnum:
		break
	    # Note: messages are completely ignored so we do not keep track
	    #       of them
	    if newMethod:
		self.methodDecl += [newMethod]
	    if newAlias:
		self.aliasDecl += [newAlias]
	    if newMixin:
		self.mixinDecl += [newMixin]
	    if newMixinReference:
		self.methodDecl += [newMixinReference]
	    if newClass:
		self.classDecl += [newClass]
	    if newGlobal:
		self.globalDecl += [newGlobal]
	    if newEnum:
		self.enumDecl += [newEnum]
	return True
    def __str__(self):
	res = "needle 2007;\n"
	for cls in self.classDecl:
	    res += str(cls)
	for meth in self.methodDecl:
	    res += str(meth)
	return res
    def generateCode(self):
	res = []
	symbolTable = {}
	for enum in self.enumDecl:
	    self.classDecl += enum.extractClasses()
	for cls in self.classDecl + self.mixinDecl + self.globalDecl:
	    symbolTable.update(cls.extractSymbols())
	for meth in self.methodDecl:
	    res += meth.generateCode(symbolTable)
	for enum in self.enumDecl:
	    res += enum.generateCode(symbolTable)
	for alias in self.aliasDecl:
	    res += alias.generateCode(symbolTable)
	for mixin in self.mixinDecl:
	    res += mixin.generateCode(symbolTable)
	for cls in self.classDecl + self.globalDecl:
	    res += cls.generateCode(symbolTable)
	return res
