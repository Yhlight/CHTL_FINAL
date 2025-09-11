grammar CHTL;

// Parser Rules
document: (directive | definition | element | originPlaceholder | scriptPlaceholder | stylePlaceholder)*;

directive
    : importStatement
    | namespaceStatement
    | configurationStatement
    ;

importStatement: IMPORT importType FROM path (AS IDENTIFIER)? SEMI;
importType: AT_HTML | AT_STYLE | AT_JAVASCRIPT | AT_CHTL;
path: STRING | IDENTIFIER;

namespaceStatement: NAMESPACE IDENTIFIER SEMI;
configurationStatement: CONFIGURATION (AT_CONFIG IDENTIFIER)? LBRACE attribute* RBRACE;

definition
    : TEMPLATE (styleTemplate | elementTemplate | varTemplate)
    | CUSTOM (styleTemplate | elementTemplate | varTemplate)
    ;

styleTemplate: AT_STYLE IDENTIFIER LBRACE attribute* RBRACE;
elementTemplate: AT_ELEMENT IDENTIFIER LBRACE element* RBRACE;
varTemplate: AT_VAR IDENTIFIER LBRACE attribute* RBRACE;

element: IDENTIFIER LBRACE (attribute | element | textNode | stylePlaceholder | scriptPlaceholder | elementUsage | originPlaceholder)* RBRACE;
elementUsage: AT_ELEMENT IDENTIFIER (FROM namespacePath)? ( (LBRACE specializationBody* RBRACE) | SEMI );
specializationBody: insertStatement | deleteStatement;
insertStatement: INSERT (insertPosition)? LBRACE element+ RBRACE;
deleteStatement: DELETE selector SEMI;

insertPosition
    : (AFTER | BEFORE | REPLACE) selector
    | AT (TOP | BOTTOM)
    ;

selector: IDENTIFIER (LBRACK NUMBER RBRACK)?;

namespacePath: IDENTIFIER (DOT IDENTIFIER)*;
attribute: IDENTIFIER ((COLON | EQ) value)? SEMI;
textNode: TEXT LBRACE value RBRACE;
stylePlaceholder: STYLE_REF LPAR STRING RPAR SEMI;
scriptPlaceholder: SCRIPT_REF LPAR STRING RPAR SEMI;
originPlaceholder: ORIGIN_REF LPAR STRING RPAR SEMI;

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
AT_HTML: '@Html';
AT_STYLE: '@Style';
AT_JAVASCRIPT: '@JavaScript';
AT_ELEMENT: '@Element';
AT_VAR: '@Var';
INSERT: 'insert';
DELETE: 'delete';
AFTER: 'after';
BEFORE: 'before';
REPLACE: 'replace';
AT: 'at';
TOP: 'top';
BOTTOM: 'bottom';
FROM: 'from';
AS: 'as';
STYLE_REF: '__style_ref__';
SCRIPT_REF: '__script_ref__';
ORIGIN_REF: '__origin_ref__';
STYLE: 'style';
TEXT: 'text';
NUMBER: [0-9]+;
DOT: '.';
LPAR: '(';
RPAR: ')';
LBRACE: '{';
RBRACE: '}';
LBRACK: '[';
RBRACK: ']';
COLON: ':';
EQ: '=';
SEMI: ';';

IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_-]*;
STRING: '"' ~["]* '"' | '\'' ~[']* '\'';
LINE_COMMENT: '//' ~[\r\n]* -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;
WS: [ \t\r\n]+ -> skip;
