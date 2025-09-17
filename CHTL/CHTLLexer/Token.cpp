#include "Token.h"

namespace CHTL {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::TOKEN_ILLEGAL: return "ILLEGAL";
        case TokenType::TOKEN_EOF: return "EOF";
        case TokenType::TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TokenType::TOKEN_STRING: return "STRING";
        case TokenType::TOKEN_NUMBER: return "NUMBER";
        case TokenType::TOKEN_EQUAL: return "=";
        case TokenType::TOKEN_COLON: return ":";
        case TokenType::TOKEN_PLUS: return "+";
        case TokenType::TOKEN_MINUS: return "-";
        case TokenType::TOKEN_STAR: return "*";
        case TokenType::TOKEN_SLASH: return "/";
        case TokenType::TOKEN_PERCENT: return "%";
        case TokenType::TOKEN_POWER: return "**";
        case TokenType::TOKEN_QUESTION: return "?";
        case TokenType::TOKEN_LOGICAL_AND: return "&&";
        case TokenType::TOKEN_LOGICAL_OR: return "||";
        case TokenType::TOKEN_AMPERSAND: return "&";
        case TokenType::TOKEN_LPAREN: return "(";
        case TokenType::TOKEN_RPAREN: return ")";
        case TokenType::TOKEN_LBRACE: return "{";
        case TokenType::TOKEN_RBRACE: return "}";
        case TokenType::TOKEN_LBRACKET: return "[";
        case TokenType::TOKEN_RBRACKET: return "]";
        case TokenType::TOKEN_COMMA: return ",";
        case TokenType::TOKEN_SEMICOLON: return ";";
        case TokenType::TOKEN_DOT: return ".";
        case TokenType::TOKEN_HASH: return "#";
        case TokenType::TOKEN_AT: return "@";
        case TokenType::TOKEN_KEYWORD_CUSTOM: return "[Custom]";
        case TokenType::TOKEN_KEYWORD_TEMPLATE: return "[Template]";
        case TokenType::TOKEN_KEYWORD_ORIGIN: return "[Origin]";
        case TokenType::TOKEN_KEYWORD_IMPORT: return "[Import]";
        case TokenType::TOKEN_KEYWORD_NAMESPACE: return "[Namespace]";
        case TokenType::TOKEN_KEYWORD_CONFIGURATION: return "[Configuration]";
        case TokenType::TOKEN_KEYWORD_INFO: return "[Info]";
        case TokenType::TOKEN_KEYWORD_EXPORT: return "[Export]";
        case TokenType::TOKEN_KEYWORD_NAME: return "[Name]";
        case TokenType::TOKEN_KEYWORD_ORIGINTYPE: return "[OriginType]";
        case TokenType::TOKEN_KEYWORD_STYLE: return "STYLE";
        case TokenType::TOKEN_KEYWORD_ELEMENT: return "ELEMENT";
        case TokenType::TOKEN_KEYWORD_VAR: return "VAR";
        case TokenType::TOKEN_KEYWORD_HTML: return "HTML";
        case TokenType::TOKEN_KEYWORD_JAVASCRIPT: return "JAVASCRIPT";
        case TokenType::TOKEN_KEYWORD_CHTL: return "CHTL";
        case TokenType::TOKEN_KEYWORD_CJMOD: return "CJMOD";
        case TokenType::TOKEN_KEYWORD_CONFIG: return "CONFIG";
        case TokenType::TOKEN_KEYWORD_TEXT: return "text";
        case TokenType::TOKEN_KEYWORD_SCRIPT: return "script";
        case TokenType::TOKEN_KEYWORD_INHERIT: return "inherit";
        case TokenType::TOKEN_KEYWORD_DELETE: return "delete";
        case TokenType::TOKEN_KEYWORD_INSERT: return "insert";
        case TokenType::TOKEN_KEYWORD_AFTER: return "after";
        case TokenType::TOKEN_KEYWORD_BEFORE: return "before";
        case TokenType::TOKEN_KEYWORD_REPLACE: return "replace";
        case TokenType::TOKEN_KEYWORD_ATTOP: return "at top";
        case TokenType::TOKEN_KEYWORD_ATBOTTOM: return "at bottom";
        case TokenType::TOKEN_KEYWORD_FROM: return "from";
        case TokenType::TOKEN_KEYWORD_AS: return "as";
        case TokenType::TOKEN_KEYWORD_EXCEPT: return "except";
        case TokenType::TOKEN_KEYWORD_USE: return "use";
        case TokenType::TOKEN_KEYWORD_HTML5: return "html5";
        case TokenType::TOKEN_COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL
