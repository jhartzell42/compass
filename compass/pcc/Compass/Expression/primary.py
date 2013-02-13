from Compass.parser import tryParse,tryParseAll
from .expression import Expression
from .identifier import LocalVariableRef, PackageVariableRef, SelfReference
from .identifier import InstanceVariableRef
from .literal import NumberLiteral,StringLiteral,SymbolLiteral,SelectorLiteral
from .literal import BlockLiteral
from sys import argv
from Compass.tokenizer import Tokenizer
from .expression import parseExpression
from Compass.exceptions import SyntaxError
from .mcall import MessageSend

def parsePrimaryExpression(tokenizer):
    return tryParseAll([InstanceVariableRef,
	LocalVariableRef,
	PackageVariableRef,
	SelfReference,
	NumberLiteral,
	StringLiteral,
	SymbolLiteral,
	SelectorLiteral,
	BlockLiteral,
	],tokenizer)
