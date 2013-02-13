from .exceptions import SyntaxError

def parse(tokenizer):
    raise NotImplementedError

def tryParse(anASTClass,tokenizer):
    res = anASTClass(tokenizer)
    if res.tryParse():
	return res
    else:
	return None

def tryParseAll(someASTClasses,tokenizer):
    for eachASTClass in someASTClasses:
	res = tryParse(eachASTClass,tokenizer)
	if res:
	    return res
    return None

class ASTNode:
    def __init__(self,tokenizer = None):
	self.tokenizer = tokenizer
