from .expression import Expression
from Compass.parser import tryParse
from .binary import parseExpressionLevel
from Compass.package import PackageSpec
from Compass.tokenizer import TokenTypes
from .identifier import SelfReference,SuperReference
from .mcall import MessageSend
class KeywordMessageSend(MessageSend):
    def __init__(self,rcvr,package,messageName,args):
	self.receiver = rcvr
	self.package = package
	self.mesgName = messageName
	self.args = args
    def __str__(self):
	res = '(#'
	if self.package:
	    res += str(self.package)
	res += self.messageName()
	res += '('
	res += str(self.receiver) + ','
	for arg in self.args:
	    res += str(arg) + ','
	res += '))'
	return res
def parseKeywordMessageSend(tokenizer):
    rcvr = parseExpressionLevel(10000,tokenizer)
    superToken = super = None
    if not rcvr:
	superToken = tokenizer.nextToken()
	if superToken and superToken.str == 'super':
	    super = rcvr = SuperReference()
	else:
	    if superToken:
		tokenizer.push(superToken)
	    superToken = None
    package = tryParse(PackageSpec,tokenizer)
    keywordId = tokenizer.nextToken()
    colon = tokenizer.checkNoWhitespace(':')
    if not keywordId or keywordId.type() != TokenTypes.identifier or not colon:
	if colon:
	    tokenizer.push(colon)
	if package:
	    package.pushBack(tokenizer)
	if keywordId:
	    tokenizer.push(keywordId)
	if superToken:
	    tokenizer.push(superToken)
	return rcvr
    # committed
    methodName = ''
    args = []
    while keywordId and keywordId.type() == TokenTypes.identifier and colon:
	methodName += keywordId.str + ':'
	args += [parseExpressionLevel(10000,tokenizer)]
	keywordId = tokenizer.nextToken()
	colon = tokenizer.checkNoWhitespace(':')
    if keywordId:
	tokenizer.push(keywordId)
    if rcvr == None:
	rcvr = SelfReference()
    return KeywordMessageSend(rcvr,package,methodName,args)
