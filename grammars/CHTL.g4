grammar CHTL;

// Parser Rules
document: element*;

element: IDENTIFIER LBRACE (attribute | element | textNode)* RBRACE;

attribute: IDENTIFIER (COLON | EQ) value SEMI;

textNode: TEXT LBRACE value RBRACE;

value: STRING | (IDENTIFIER | TEXT)+ ;


// Lexer Rules
TEXT: 'text';

LBRACE: '{';
RBRACE: '}';
COLON: ':';
EQ: '=';
SEMI: ';';

IDENTIFIER: [a-zA-Z_] [a-zA-Z0-9_-]*;

// NOTE: This is a simplified string rule that does not support escaped quotes.
STRING
    : '"' ~["]* '"'
    | '\'' ~[']* '\''
    ;

LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;

WS: [ \t\r\n]+ -> skip;
