grammar Alice;

/* Parser rules */
aliceScript: statements EOF;

statements: (statement SEMIS)+;
statement: usingNamespace;

usingNamespace: USING NAMESPACE identifierPath;

identifierPath: '::'? identifier ('::' identifier)*;
identifier: KEYWORD | IDENTIFIER;

fragment SEMIS: ';' | NL (';' | NL)*;
fragment KEYWORD:
	ID
	| PROP
	| QUERY
	| INCLUDE
	| NAMESPACE
	| CONST
	| AND
	| OR
	| USING
	| NOT;

/* Lexer rules */
NL: LF | (CR LF?);

BLOCK_COMMENT: '/*' ( BLOCK_COMMENT | .)* '*/';
LINE_COMMENT: '//' ~[NL]*;
WS: ' ' | '\t' | '\f';
SKIPPED: (BLOCK_COMMENT | LINE_COMMENT | WS) -> skip;

IDENTIFIER: '_' | [a-zA-Z][a-zA-Z0-9_]*;

/* Keywords */
ID: 'id';
PROP: 'prop';
QUERY: 'query';
INCLUDE: 'include';
NAMESPACE: 'namespace';
CONST: 'const';
AND: 'and';
OR: 'or';
USING: 'using';
NOT: 'not';

fragment LF: '\n';
fragment CR: '\r';