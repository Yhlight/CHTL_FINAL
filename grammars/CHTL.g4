grammar CHTL;

// Parser Rules
document: (definition | element)*;

definition
    : TEMPLATE (styleTemplate | elementTemplate | varTemplate)
    | CUSTOM (styleTemplate | elementTemplate | varTemplate)
    ;

styleTemplate: AT_STYLE IDENTIFIER LBRACE styleContent* RBRACE;
elementTemplate: AT_ELEMENT IDENTIFIER LBRACE element* RBRACE;
varTemplate: AT_VAR IDENTIFIER LBRACE attribute* RBRACE;


element: IDENTIFIER LBRACE (attribute | element | textNode | stylePlaceholder | elementUsage)* RBRACE;
elementUsage: AT_ELEMENT IDENTIFIER (LBRACE RBRACE)? SEMI; // For now, only empty body for customization placeholder

attribute: IDENTIFIER (COLON | EQ) value SEMI;

textNode: TEXT LBRACE value RBRACE;

stylePlaceholder: STYLE_REF LPAR STRING RPAR SEMI;

styleContent
    : IDENTIFIER | STRING | COLON | SEMI | TEXT | LBRACE | EQ | NUMBER | DOT
    ;

value: STRING | varUsage | (IDENTIFIER | TEXT | STYLE)+ ;
varUsage: IDENTIFIER LPAR IDENTIFIER (EQ value)? RPAR;


// Lexer Rules
TEMPLATE: '[Template]';
CUSTOM: '[Custom]';
AT_STYLE: '@Style';
AT_ELEMENT: '@Element';
AT_VAR: '@Var';

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

STRING
    : '"' ~["]* '"'
    | '\'' ~[']* '\''
    ;

LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;

WS: [ \t\r\n]+ -> skip;
