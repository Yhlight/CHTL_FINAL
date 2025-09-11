grammar CHTL;

// Parser Rules
document: element*;

element: IDENTIFIER LBRACE (attribute | element | textNode | stylePlaceholder)* RBRACE;

attribute: IDENTIFIER (COLON | EQ) value SEMI;

textNode: TEXT LBRACE value RBRACE;

stylePlaceholder: STYLE_REF LPAR STRING RPAR SEMI;

value: STRING | (IDENTIFIER | TEXT | STYLE)+ ;


// Lexer Rules
STYLE_REF: '__style_ref__';
STYLE: 'style';
TEXT: 'text';
NUMBER: [0-9]+;
DOT: '.';

LPAR: '(';
RPAR: ')';
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
