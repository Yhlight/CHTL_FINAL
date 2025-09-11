grammar CSSValueExpression;

// Entry point for an expression
parse: expression;

// Expression hierarchy for precedence
expression
    : logicalOrExpression (QUESTION expression COLON expression)?
    ;

logicalOrExpression
    : logicalAndExpression (OR logicalAndExpression)*
    ;

logicalAndExpression
    : equalityExpression (AND equalityExpression)*
    ;

equalityExpression
    : relationalExpression ((EQ | NEQ) relationalExpression)*
    ;

relationalExpression
    : additiveExpression ((GT | GTE | LT | LTE) additiveExpression)*
    ;

additiveExpression
    : multiplicativeExpression ((ADD | SUB) multiplicativeExpression)*
    ;

multiplicativeExpression
    : powerExpression ((MUL | DIV | MOD) powerExpression)*
    ;

powerExpression
    : unaryExpression (POW unaryExpression)*
    ;

unaryExpression
    : (ADD | SUB) unaryExpression
    | atom
    ;

atom
    : propertyReference
    | literal
    | LPAR expression RPAR
    ;

literal
    : DIMENSION
    | NUMBER
    | STRING
    | COLOR
    | IDENTIFIER // For keywords like 'red', 'solid', 'auto'
    ;

propertyReference
    : IDENTIFIER (DOT IDENTIFIER)*
    ;

// --- Lexer Rules ---

// Operators
ADD: '+';
SUB: '-';
MUL: '*';
DIV: '/';
MOD: '%';
POW: '**';

GT: '>';
GTE: '>=';
LT: '<';
LTE: '<=';
EQ: '==';
NEQ: '!=';

AND: '&&';
OR: '||';

LPAR: '(';
RPAR: ')';
DOT: '.';
QUESTION: '?';
COLON: ':';

fragment HEX_DIGIT: [0-9a-fA-F];

// Literals
DIMENSION: NUMBER ('px' | 'em' | 'rem' | '%' | 'vw' | 'vh' | 'deg');
NUMBER: [0-9]+ ('.' [0-9]+)? | '.' [0-9]+;
STRING: '"' (~["\r\n])* '"' | '\'' (~['\r\n])* '\'';
COLOR: '#' (
    HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT |
    HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT |
    HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT |
    HEX_DIGIT HEX_DIGIT HEX_DIGIT
);
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_-]*;

// Whitespace
WS: [ \t\r\n]+ -> skip;
