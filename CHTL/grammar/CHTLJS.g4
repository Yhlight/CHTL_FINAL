grammar CHTLJS;

// 语法规则
program: statement* EOF;

// 语句
statement: expressionStatement | declarationStatement | controlStatement | chthljsStatement;

expressionStatement: expression SEMICOLON?;
declarationStatement: variableDeclaration | functionDeclaration;
controlStatement: ifStatement | forStatement | whileStatement | switchStatement;
chthljsStatement: fileloaderStatement | listenStatement | delegateStatement | animateStatement | virStatement | routerStatement | utilStatement;

// 表达式
expression: assignmentExpression;
assignmentExpression: conditionalExpression (assignmentOperator assignmentExpression)?;
conditionalExpression: logicalOrExpression (QUESTION expression COLON conditionalExpression)?;
logicalOrExpression: logicalAndExpression (OR logicalAndExpression)*;
logicalAndExpression: equalityExpression (AND equalityExpression)*;
equalityExpression: relationalExpression ((EQUAL_EQUAL | NOT_EQUAL) relationalExpression)*;
relationalExpression: additiveExpression ((LESS | GREATER | LESS_EQUAL | GREATER_EQUAL) additiveExpression)*;
additiveExpression: multiplicativeExpression ((PLUS | MINUS) multiplicativeExpression)*;
multiplicativeExpression: unaryExpression ((MULTIPLY | DIVIDE | MODULO) unaryExpression)*;
unaryExpression: (PLUS | MINUS | NOT | TILDE) unaryExpression | postfixExpression;
postfixExpression: primaryExpression (LEFT_BRACKET expression RIGHT_BRACKET | DOT IDENTIFIER | LEFT_PAREN argumentList? RIGHT_PAREN)*;

// 主要表达式
primaryExpression: literal | identifier | chthljsSelector | functionExpression | objectExpression | arrayExpression | parenthesizedExpression;

// 字面量
literal: STRING | NUMBER | BOOLEAN | NULL | UNDEFINED;

// 标识符
identifier: IDENTIFIER;

// CHTL JS选择器
chthljsSelector: DOUBLE_BRACE selectorContent DOUBLE_BRACE;
selectorContent: selector | selector DOT property | selector LEFT_BRACKET index RIGHT_BRACKET;
selector: IDENTIFIER | HASH IDENTIFIER | DOT IDENTIFIER | ASTERISK | IDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET;
property: IDENTIFIER;
index: NUMBER | IDENTIFIER;

// 函数表达式
functionExpression: FUNCTION LEFT_PAREN parameterList? RIGHT_PAREN block;
parameterList: IDENTIFIER (COMMA IDENTIFIER)*;

// 对象表达式
objectExpression: LEFT_BRACE (propertyAssignment (COMMA propertyAssignment)*)? RIGHT_BRACE;
propertyAssignment: propertyName COLON expression;
propertyName: IDENTIFIER | STRING | LEFT_BRACKET expression RIGHT_BRACKET;

// 数组表达式
arrayExpression: LEFT_BRACKET (expression (COMMA expression)*)? RIGHT_BRACKET;

// 括号表达式
parenthesizedExpression: LEFT_PAREN expression RIGHT_PAREN;

// 变量声明
variableDeclaration: (VAR | LET | CONST) variableDeclarator (COMMA variableDeclarator)*;
variableDeclarator: IDENTIFIER (EQUAL expression)?;

// 函数声明
functionDeclaration: FUNCTION IDENTIFIER LEFT_PAREN parameterList? RIGHT_PAREN block;

// 控制语句
ifStatement: IF LEFT_PAREN expression RIGHT_PAREN statement (ELSE statement)?;
forStatement: FOR LEFT_PAREN (variableDeclaration | expressionStatement | SEMICOLON) expression? SEMICOLON expression? RIGHT_PAREN statement;
whileStatement: WHILE LEFT_PAREN expression RIGHT_PAREN statement;
switchStatement: SWITCH LEFT_PAREN expression RIGHT_PAREN LEFT_BRACE caseClause* defaultClause? RIGHT_BRACE;
caseClause: CASE expression COLON statement*;
defaultClause: DEFAULT COLON statement*;

// CHTL JS语句
fileloaderStatement: FILELOADER LEFT_PAREN STRING (COMMA STRING)* RIGHT_PAREN block;
listenStatement: LISTEN LEFT_PAREN chthljsSelector COMMA functionExpression RIGHT_PAREN;
delegateStatement: DELEGATE LEFT_PAREN chthljsSelector COMMA STRING COMMA functionExpression RIGHT_PAREN;
animateStatement: ANIMATE LEFT_PAREN chthljsSelector COMMA functionExpression RIGHT_PAREN;
virStatement: VIR LEFT_PAREN chthljsSelector RIGHT_PAREN;
routerStatement: ROUTER LEFT_PAREN STRING COMMA functionExpression RIGHT_PAREN;
utilStatement: UTIL expression THEN statement;

// 块
block: LEFT_BRACE statement* RIGHT_BRACE;

// 参数列表
argumentList: expression (COMMA expression)*;

// 赋值操作符
assignmentOperator: EQUAL | PLUS_EQUAL | MINUS_EQUAL | MULTIPLY_EQUAL | DIVIDE_EQUAL | MODULO_EQUAL;

// 词法规则
// 关键字
VAR: 'var';
LET: 'let';
CONST: 'const';
FUNCTION: 'function';
IF: 'if';
ELSE: 'else';
FOR: 'for';
WHILE: 'while';
SWITCH: 'switch';
CASE: 'case';
DEFAULT: 'default';
BREAK: 'break';
CONTINUE: 'continue';
RETURN: 'return';
THEN: 'then';

// CHTL JS关键字
FILELOADER: 'fileloader';
LISTEN: 'listen';
DELEGATE: 'delegate';
ANIMATE: 'animate';
VIR: 'vir';
ROUTER: 'router';
UTIL: 'util';
INEVERAWAY: 'iNeverAway';

// 分隔符
LEFT_BRACE: '{';
RIGHT_BRACE: '}';
LEFT_BRACKET: '[';
RIGHT_BRACKET: ']';
LEFT_PAREN: '(';
RIGHT_PAREN: ')';
SEMICOLON: ';';
COMMA: ',';
DOT: '.';
COLON: ':';
QUESTION: '?';

// 运算符
PLUS: '+';
MINUS: '-';
MULTIPLY: '*';
DIVIDE: '/';
MODULO: '%';
AND: '&&';
OR: '||';
NOT: '!';
GREATER: '>';
LESS: '<';
GREATER_EQUAL: '>=';
LESS_EQUAL: '<=';
EQUAL_EQUAL: '==';
NOT_EQUAL: '!=';
PLUS_EQUAL: '+=';
MINUS_EQUAL: '-=';
MULTIPLY_EQUAL: '*=';
DIVIDE_EQUAL: '/=';
MODULO_EQUAL: '%=';

// 特殊符号
HASH: '#';
ASTERISK: '*';
DOUBLE_BRACE: '{{' | '}}';

// 字面量
STRING: '"' (~["\\] | '\\' .)* '"' | '\'' (~['\\] | '\\' .)* '\'';
NUMBER: [0-9]+ ('.' [0-9]+)? ([eE] [+-]? [0-9]+)?;
BOOLEAN: 'true' | 'false';
NULL: 'null';
UNDEFINED: 'undefined';

// 标识符
IDENTIFIER: [a-zA-Z_$][a-zA-Z0-9_$]*;

// 注释
COMMENT: '//' ~[\r\n]* -> skip;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip;

// 空白
WHITESPACE: [ \t\r\n]+ -> skip;