#include "CHTL/CHTLLexer/Token.h"
#include <sstream>

namespace CHTL {

bool Token::isKeyword() const {
    switch (type) {
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
        case TokenType::ORIGIN:
        case TokenType::IMPORT:
        case TokenType::CONFIGURATION:
        case TokenType::NAMESPACE:
        case TokenType::STYLE:
        case TokenType::ELEMENT:
        case TokenType::VAR:
        case TokenType::HTML:
        case TokenType::JAVASCRIPT:
        case TokenType::CHTL:
        case TokenType::CJMOD:
        case TokenType::TEXT:
        case TokenType::SCRIPT:
        case TokenType::INHERIT:
        case TokenType::DELETE:
        case TokenType::INSERT:
        case TokenType::AFTER:
        case TokenType::BEFORE:
        case TokenType::REPLACE:
        case TokenType::AT_TOP:
        case TokenType::AT_BOTTOM:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::EXCEPT:
        case TokenType::USE:
        case TokenType::HTML5:
            return true;
        default:
            return false;
    }
}

bool Token::isOperator() const {
    switch (type) {
        case TokenType::COLON:
        case TokenType::EQUALS:
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::LBRACE:
        case TokenType::RBRACE:
        case TokenType::LBRACKET:
        case TokenType::RBRACKET:
        case TokenType::LPAREN:
        case TokenType::RPAREN:
        case TokenType::AT_SYMBOL:
        case TokenType::HASH:
        case TokenType::DOT:
        case TokenType::AMPERSAND:
        case TokenType::QUESTION:
        case TokenType::EXCLAMATION:
        case TokenType::UNDERSCORE:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::NOT:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
            return true;
        default:
            return false;
    }
}

bool Token::isDelimiter() const {
    switch (type) {
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::LBRACE:
        case TokenType::RBRACE:
        case TokenType::LBRACKET:
        case TokenType::RBRACKET:
        case TokenType::LPAREN:
        case TokenType::RPAREN:
            return true;
        default:
            return false;
    }
}

std::string Token::getTypeName() const {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::LINE_COMMENT: return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::STYLE: return "STYLE";
        case TokenType::ELEMENT: return "ELEMENT";
        case TokenType::VAR: return "VAR";
        case TokenType::HTML: return "HTML";
        case TokenType::JAVASCRIPT: return "JAVASCRIPT";
        case TokenType::CHTL: return "CHTL";
        case TokenType::CJMOD: return "CJMOD";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::AT_SYMBOL: return "AT_SYMBOL";
        case TokenType::HASH: return "HASH";
        case TokenType::DOT: return "DOT";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::UNDERSCORE: return "UNDERSCORE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::TEXT: return "TEXT";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::USE: return "USE";
        case TokenType::HTML5: return "HTML5";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << getTypeName() << ", \"" << value << "\", " 
        << line << ":" << column << ")";
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << token.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const TokenList& tokenList) {
    os << "TokenList[";
    for (size_t i = 0; i < tokenList.size(); ++i) {
        if (i > 0) os << ", ";
        os << tokenList[i];
    }
    os << "]";
    return os;
}

} // namespace CHTL