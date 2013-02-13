from __future__ import with_statement
from sys import argv
from Compass.tokenizer import Tokenizer
from Compass.Expression.expression import parseExpression

if __name__ == '__main__':
    with open(argv[1]) as file:
	contents = ''
	for line in file:
	    contents += line
	tokenizer = Tokenizer(contents)
	res = 'Expressions:'
	while res:
	    print res
	    res = parseExpression(tokenizer)
	t = tokenizer.nextToken()
	if t:
	    print 'Error: tokens left over:',t.errForm()
