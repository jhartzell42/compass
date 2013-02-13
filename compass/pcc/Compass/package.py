from __future__ import with_statement
from .parser import ASTNode,tryParse,tryParseAll
from .tokenizer import Tokenizer,TokenTypes
from sys import argv
from re import compile

class PackageSpec(ASTNode):
    def tryParse(self):
	tokens = []
	token = self.tokenizer.nextToken()
	if not token:
	    return False
	tokens += [token]
	self.absolute = False
	if token.str == '::':
	    self.absolute = True
	    token = self.tokenizer.nextToken()
	    tokens += [token]
	self.pkgs = []
	pkg = token
	tokens += [pkg]
	while pkg:
	    if pkg.type() != TokenTypes.identifier and pkg.str != '..':
		self.tokenizer.push(pkg)
		tokens = tokens[:-1]
		break
	    separator = self.tokenizer.nextToken()
	    if not separator or not (separator.str in ('!!','::')):
		if separator:
		    self.tokenizer.push(separator)
		self.tokenizer.push(pkg)
		tokens = tokens[:-2]
		break
	    else:
		tokens += [separator]
	    self.pkgs += [pkg.str]
	    if separator.str == '!!':
		self.pkgs += ['__bang__']
	    pkg = self.tokenizer.nextToken()
	    tokens += [pkg]
	self.tokens = tokens
	self.tokens.reverse()
	return self.pkgs or self.absolute
    def __str__(self):
	if self.absolute:
	    res = '::'
	else:
	    res = ''
	for pkg in self.pkgs:
	    res += pkg+'::'
	return res
    def pushBack(self,tokenizer):
	for token in tokens:
	    tokenizer.push(token)
