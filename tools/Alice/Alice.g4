grammar Alice;
aliceFile: expr EOF;
expr:
	expr ('*' | '/') expr
	| expr ('+' | '-') expr
	| INT
	| '(' expr ')';
INT: [0-9]+;