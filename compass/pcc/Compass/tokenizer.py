from __future__ import with_statement
from re import compile, escape
from .exceptions import SyntaxError
from sys import argv

class TokenizerError(SyntaxError):
    pass

identifier_regex = compile("[a-zA-Z][0-9a-zA-Z]*")
number_regex = compile("[0-9]+n?")

class TokenTypes:
    number,string,identifier,operator = range(4)

class Token:
    def __init__(self, token, file, line, col, whitespace):
	self.str = token
	self.file = file
	self.line = line
	self.col = col
	self.ws = whitespace
    def whitespace(self):
	return self.ws
    def type(self):
	if number_regex.match(self.str):
	    return TokenTypes.number
	if identifier_regex.match(self.str):
	    return TokenTypes.identifier
	if self.str[0]=="'":
	    return TokenTypes.string
	return TokenTypes.operator
    def errForm(self):
	if self.file:
	    return "%s(%s:%d:%d)" % (self.str, self.file, self.line, self.col)
	else:
	    return "%s(%d:%d)" % (self.str, self.line, self.col)
    def __str__(self):
	return 'Token: '+self.errForm()

def default_ops():
    def compare(a,b):
	return cmp(len(b),len(a))
    ops = ["{",
	   "}",
	   "_",
	   ",",
	   "$",
	   "@",
	   "%",
	   "!",
	   "#",
	   "::",
	   "#[]",
	   "#[~]",
	   "#[~~]",
	   "#:[]=",
	   "#:[~]=",
	   "#:[~~]=",
	   "#**",
	   "#*",
	   "#/",
	   "#\\",
	   "#%",
	   "#+",
	   "#-",
	   "#<<",
	   "#>>",
	   "#>>>",
	   "#&",
	   "#|",
	   "#...",
	   "#.",
	   "#->",
	   "#|->",
	   "#><",
	   "#<",
	   "#<=",
	   "#>",
	   "#>=",
	   "#=",
	   "#!=",
	   "#==",
	   "#!==",
	   "#===",
	   "#!===",
	   "#<=>",
	   "#=>",
	   "#&&",
	   "#||",
	   "#?//",
	   "#<--",
	   "#()",
	   "--",
	   "!!",
	   "..",
	   "@@",
	   "(",
	   ")",
	   "[",
	   "]",
	   "~",
	   ":[",
	   "]=",
	   "**",
	   "*",
	   "/",
	   "\\",
	   "%",
	   "+",
	   "-",
	   "<<",
	   ">>",
	   ">>>",
	   "&",
	   "|",
	   "...",
	   ".",
	   "->",
	   "|->",
	   "><",
	   "<",
	   "<=",
	   ">",
	   ">=",
	   "=",
	   "!=",
	   "==",
	   "!==",
	   "===",
	   "!===",
	   "<=>",
	   "=>",
	   "&&",
	   "||",
	   ":=",
	   "_",
	   "^",
	   ";",
	   "?",
	   "//",
	   "<--",
	   "[[",
	   "]]",
	   ":"]
    ops.sort(compare)
    return ops

class Tokenizer:
    def __init__(self,str,filename = None):
	self._stringRemaining = str
	self._ops = default_ops()
	self._strings = True
	self._filename = filename
	self._currentLine = 1
	self._currentCol = 1
	self._peek = []
	self._g = 0
    def setOps(self,ops):
	self._ops = ops
    def allowStrings(self):
	self._strings = True
    def forbidStrings(self):
	self._strings = False
    def push(self,pushback):
	if pushback == None:
	    raise TokenizerError("can't pushback nothing")
	self._peek += [pushback]
    def nextToken(self):
	if len(self._peek)>0:
	    res = self._peek[-1]
	    self._peek = self._peek[:-1]
	    if res == None:
		print 'wtf1? tokenizer.py'
	else:
	    res = self._nextToken()
	return res
    def _nextToken(self):
	wspace = self._stripWhitespace()
	if self._length() == 0:
	    return None
	(file,line,col) = (self._filename,self._currentLine,self._currentCol)
	str = self._nextStr()
	return Token(str, file, line, col, wspace)
    def next(self):
	res = self.nextToken()
	if res:
	    return res
	raise StopIteration()
    def _grabOperator(self):
	for op in self._ops:
	    length = len(op)
	    if self._stringRemaining[0:length] == op:
		self._stringRemaining = self._stringRemaining[length:]
		self._advance(length)
		return op
	return False
    def peek(self):
	token = self.nextToken()
	if token:
	    self.push(token)
	return token
    def _fail(self):
	raise TokenizerError(str(self._currentLine) +
		":" +
		str(self._currentCol))
    def _grabId(self):
	match = identifier_regex.match(self._stringRemaining)
	if match == None:
	    return None
	id = match.group(0)
	length = len(id)
	self._advance(length)
	self._stringRemaining = self._stringRemaining[length:]
	return id
    def _grabNumber(self):
	match = number_regex.match(self._stringRemaining)
	if match == None:
	    return None
	num = match.group(0)
	length = len(num)
	self._advance(length)
	self._stringRemaining = self._stringRemaining[length:]
	return num
    def checkGeneric(self,func):
	token = self.nextToken()
	if not token:
	    return None
	if func(token):
	    return token
	else:
	    self.push(token)
	    return None
    def check(self,str):
	def checkFunc(token):
	    return token.str == str
	return self.checkGeneric(checkFunc)
    def checkNoWhitespace(self,str = None):
	def checkFunc(token):
	    if token.whitespace():
		return False
	    if str != None and str != token.str:
		return False
	    return True
	return self.checkGeneric(checkFunc)
    def checkRegex(self,regex):
	def checkFunc(token):
	    return regex.match(token.str)
	return self.checkGeneric(checkFunc)
    def peekCheck(self,str):
	token = self.check(str)
	if token:
	    self.push(token)
	    return True
	else:
	    return False
    def require(self,str):
	if not self.check(str):
	    raise SyntaxError('Expected %s; found: %s' % \
		    (str, self.nextToken().errForm()))
    def _nextStr(self):
	if self._strings and self._stringRemaining[0]=="'":
	    return self._eatString()
	op = self._grabOperator()
	if op:
	    return op
	id = self._grabId()
	if id:
	    return id
	num = self._grabNumber()
	if num:
	    return num
	self._fail()
    def _nextLine(self):
	self._currentCol = 1
	self._currentLine += 1
    def _advance(self,amount=1):
	self._currentCol += amount
    def _length(self):
	return len(self._stringRemaining)
    def _stripWhitespace(self):
	i = -1
	while i < self._length():
	    i += 1
	    if i == self._length():
		break
	    if self._stringRemaining[i] == "\n":
		self._nextLine()
	    elif self._stringRemaining[i] in (' ','\t'):
		self._advance()
	    elif self._stringRemaining[i] == '"':
		self._advance()
		while i < self._length():
		    i += 1
		    self._advance()
		    if self._stringRemaining[i] == "\n":
			self._nextLine()
		    elif self._stringRemaining[i] == '"':
			break
	    else:
		break
	self._stringRemaining = self._stringRemaining[i:]
	return i != 0
    def _eatString(self):
	res = "'"
	i = 1
	self._advance()
	while i < self._length():
	    self._advance()
	    if self._stringRemaining[i:i+2] == "''":
		res += "'"
		i += 2
		self._advance()
	    elif self._stringRemaining[i] == "'":
		i += 1
		break
	    else:
		if self._stringRemaining[i] == "\n":
		    self._nextLine()
		res += self._stringRemaining[i]
		i += 1
	self._stringRemaining = self._stringRemaining[i:]
	return res

class MultiFileTokenizer(Tokenizer):
    def __init__(self,files):
	self._files = files
	self._tokenizer = None
	self._ops = default_ops()
	self._strings = True
	self._peek = []

    def _readFileToStr(self,filename):
	contents = ''
	with open(filename) as file:
	    for line in file:
		contents += line
	return contents

    def _updateTokenizer(self):
	if self._tokenizer is None:
	    if len(self._files) > 0:
		filename = self._files[0]
		contents = self._readFileToStr(filename)
		self._files = self._files[1:]
		self._tokenizer = Tokenizer(contents,filename)
		self._tokenizer.setOps(self._ops)
		if self._strings:
		    self._tokenizer.allowStrings()
		else:
		    self._tokenizer.forbidStrings()
    
    # Tokenizer settings
    def setOps(self,ops):
	self._ops = ops
	self._tokenizer.setOps(ops)
    def allowStrings(self):
	self._strings = True
	self._tokenizer.allowStrings()
    def forbidStrings(self):
	self._strings = False
	self._tokenizer.forbidStrings()

    # Tokenizer calls
    def _nextToken(self):
	self._updateTokenizer()
	if self._tokenizer is None:
	    return None
	else:
	    res = self._tokenizer.nextToken()
	    if res is None:
		self._tokenizer = None
		return self.nextToken()
	    return res;

if __name__ == '__main__':
    fileContents = ''
    with open(argv[1]) as file:
	for line in file:
	    fileContents += line

    tokenizer = Tokenizer(fileContents)
    token = tokenizer.next()
    while token:
	print token
	token = tokenizer.nextToken()
