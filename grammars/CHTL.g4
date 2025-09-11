grammar CHTL;

// Parser Rules
document: (directive | definition | element)*;

directive
    : importStatement
    | namespaceStatement
    | configurationStatement
    ;

importStatement: IMPORT AT_CHTL FROM (STRING | IDENTIFIER) SEMI;
namespaceStatement: NAMESPACE IDENTIFIER SEMI;
configurationStatement: CONFIGURATION (AT_CONFIG IDENTIFIER)? LBRACE attribute* RBRACE;

definition
    : TEMPLATE (styleTemplate | elementTemplate | varTemplate)
    | CUSTOM (styleTemplate | elementTemplate | varTemplate)
    ;

styleTemplate: AT_STYLE IDENTIFIER LBRACE styleContent* RBRACE;
elementTemplate: AT_ELEMENT IDENTIFIER LBRACE element* RBRACE;
varTemplate: AT_VAR IDENTIFIER LBRACE attribute* RBRACE;

element: IDENTIFIER LBRACE (attribute | element | textNode | stylePlaceholder | scriptPlaceholder | elementUsage)* RBRACE;
elementUsage: AT_ELEMENT IDENTIFIER ( (LBRACE specializationBody* RBRACE) | SEMI );
specializationBody: insertStatement | deleteStatement;
insertStatement: INSERT LBRACE element+ RBRACE;
deleteStatement: DELETE IDENTIFIER SEMI;

namespacePath: IDENTIFIER (DOT IDENTIFIER)*;
attribute: IDENTIFIER (COLON | EQ) value SEMI;
textNode: TEXT LBRACE value RBRACE;
stylePlaceholder: STYLE_REF LPAR STRING RPAR SEMI;
scriptPlaceholder: SCRIPT_REF LPAR STRING RPAR SEMI;

styleContent: (IDENTIFIER | STRING | COLON | SEMI | TEXT | LBRACE | EQ | NUMBER | DOT | varUsage);
value: (valuePart)+ ;
valuePart: STRING | varUsage | IDENTIFIER | TEXT | STYLE | NUMBER | DOT;
varUsage: IDENTIFIER LPAR IDENTIFIER (EQ value)? RPAR;


// Lexer Rules
IMPORT: '[Import]';
NAMESPACE: '[Namespace]';
CONFIGURATION: '[Configuration]';
TEMPLATE: '[Template]';
CUSTOM: '[Custom]';
AT_CHTL: '@Chtl';
AT_CONFIG: '@Config';
AT_STYLE: '@Style';
AT_ELEMENT: '@Element';
AT_VAR: '@Var';
INSERT: 'insert';
DELETE: 'delete';
FROM: 'from';
AS: 'as';
STYLE_REF: '__style_ref__';
SCRIPT_REF: '__script_ref__';
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

IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_-]*;
STRING: '"' ~["]* '"' | '\'' ~[']* '\'';
LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;
WS: [ \t\r\n]+ -> skip;
