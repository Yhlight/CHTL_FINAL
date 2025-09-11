grammar CHTLJS;

program: statement* EOF;

statement: expression ';'? ;

expression: selector ;

selector: LDBLACE LDBLACE selectorContent RDBLACE RDBLACE;

selectorContent: (IDENTIFIER | DOT | HASH)+ ;

LDBLACE: '{';
RDBLACE: '}';
DOT: '.';
HASH: '#';

// Match common JS-like identifiers, including hyphens for CSS classes/ids
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_-]*;

// Skip whitespace
WS: [ \t\r\n]+ -> skip;
