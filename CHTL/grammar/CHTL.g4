grammar CHTL;

// 语法规则
program: (element | template | custom | origin | importStatement | namespace | configuration | useStatement | comment)* EOF;

// 元素
element: htmlElement | textElement | styleElement | scriptElement;

htmlElement: IDENTIFIER attributes? block?;
textElement: TEXT block;
styleElement: STYLE block;
scriptElement: SCRIPT block;

attributes: attribute+;
attribute: IDENTIFIER (COLON | EQUAL) value SEMICOLON?;
value: STRING | UNQUOTED_LITERAL | NUMBER | expression;

block: LEFT_BRACE (element | text | comment)* RIGHT_BRACE;

// 文本内容
text: TEXT_CONTENT;

// 模板
template: TEMPLATE templateType IDENTIFIER block;
templateType: STYLE | ELEMENT | VAR;

// 自定义
custom: CUSTOM customType IDENTIFIER block;
customType: STYLE | ELEMENT | VAR;

// 原始嵌入
origin: ORIGIN originType IDENTIFIER block;
originType: HTML | CSS | JAVASCRIPT | VUE;

// 导入
importStatement: IMPORT importType? IDENTIFIER FROM path (AS IDENTIFIER)?;
importType: TEMPLATE | CUSTOM | ORIGIN | CONFIGURATION;
path: STRING | UNQUOTED_LITERAL;

// 命名空间
namespace: NAMESPACE IDENTIFIER block;

// 配置
configuration: CONFIGURATION IDENTIFIER? block;

// 使用
useStatement: USE (HTML5 | IDENTIFIER);

// 注释
comment: COMMENT | GENERATOR_COMMENT;

// 表达式
expression: term (operator term)*;
term: IDENTIFIER | STRING | UNQUOTED_LITERAL | NUMBER | functionCall | propertyAccess | indexAccess;
functionCall: IDENTIFIER LEFT_PAREN (expression (COMMA expression)*)? RIGHT_PAREN;
propertyAccess: IDENTIFIER DOT IDENTIFIER;
indexAccess: IDENTIFIER LEFT_BRACKET expression RIGHT_BRACKET;
operator: PLUS | MINUS | MULTIPLY | DIVIDE | MODULO | AND | OR | NOT | GREATER | LESS | GREATER_EQUAL | LESS_EQUAL | EQUAL_EQUAL | NOT_EQUAL;

// 词法规则
// 关键字
TEMPLATE: '[Template]';
CUSTOM: '[Custom]';
ORIGIN: '[Origin]';
IMPORT: '[Import]';
NAMESPACE: '[Namespace]';
CONFIGURATION: '[Configuration]';
INFO: '[Info]';
EXPORT: '[Export]';

// CHTL关键字
TEXT: 'text';
STYLE: 'style';
SCRIPT: 'script';
USE: 'use';
EXCEPT: 'except';
DELETE: 'delete';
INSERT: 'insert';
INHERIT: 'inherit';
FROM: 'from';
AS: 'as';

// CHTL JS关键字
FILELOADER: 'fileloader';
LISTEN: 'listen';
DELEGATE: 'delegate';
ANIMATE: 'animate';
VIR: 'vir';
ROUTER: 'router';
UTIL: 'util';
INEVERAWAY: 'iNeverAway';

// 位置关键字
AFTER: 'after';
BEFORE: 'before';
REPLACE: 'replace';
AT_TOP: 'at top';
AT_BOTTOM: 'at bottom';

// 类型关键字
HTML: 'HTML';
CSS: 'CSS';
JAVASCRIPT: 'JavaScript';
VUE: 'Vue';
HTML5: 'HTML5';

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
EQUAL: '=';

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

// 特殊符号
AT: '@';
HASH: '#';
DOLLAR: '$';
QUESTION: '?';
EXCLAMATION: '!';
AMPERSAND: '&';
PIPE: '|';
TILDE: '~';
CARET: '^';
ARROW: '->';

// 字面量
STRING: '"' (~["\\] | '\\' .)* '"' | '\'' (~['\\] | '\\' .)* '\'';
UNQUOTED_LITERAL: [a-zA-Z_][a-zA-Z0-9_-]*;
NUMBER: [0-9]+ ('.' [0-9]+)?;

// 标识符
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*;

// 注释
COMMENT: '//' ~[\r\n]* -> skip;
GENERATOR_COMMENT: '--' ~[\r\n]* -> skip;
MULTILINE_COMMENT: '/*' .*? '*/' -> skip;

// 空白
WHITESPACE: [ \t\r\n]+ -> skip;

// 文本内容
TEXT_CONTENT: ~[{}[\];]+;