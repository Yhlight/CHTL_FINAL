#include "Token.h"
#include <string>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL:            return "ILLEGAL";
        case TokenType::END_OF_FILE:        return "END_OF_FILE";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::STRING_LITERAL:     return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL:   return "UNQUOTED_LITERAL";
        case TokenType::LEFT_BRACE:         return "{";
        case TokenType::RIGHT_BRACE:        return "}";
        case TokenType::COLON:              return ":";
        case TokenType::SEMICOLON:          return ";";
        case TokenType::EQUALS:             return "=";
        case TokenType::AMPERSAND:          return "&";
        case TokenType::DOT:                return ".";
        case TokenType::POUND:              return "#";
        case TokenType::AT_SIGN:            return "@";
        case TokenType::LEFT_BRACKET:       return "[";
        case TokenType::RIGHT_BRACKET:      return "]";
        case TokenType::LEFT_PAREN:         return "(";
        case TokenType::RIGHT_PAREN:        return ")";
        case TokenType::COMMA:              return ",";
        case TokenType::HASH_COMMENT:       return "HASH_COMMENT";
        case TokenType::KEYWORD_DELETE:     return "delete";
        case TokenType::KEYWORD_INSERT:     return "insert";
        case TokenType::KEYWORD_AFTER:      return "after";
        case TokenType::KEYWORD_BEFORE:     return "before";
        case TokenType::KEYWORD_REPLACE:    return "replace";
        case TokenType::KEYWORD_AT:         return "at";
        case TokenType::KEYWORD_TOP:        return "top";
        case TokenType::KEYWORD_BOTTOM:     return "bottom";
        default:                            return "UNKNOWN";
    }
}

std::string Token::toString() const {
    return "Token[type: " + tokenTypeToString(type) + ", literal: '" + literal + "', line: " + std::to_string(line) + "]";
}
