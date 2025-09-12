#include "Token.h"

namespace CHTL {

std::string Token::typeToString(TokenType type) {
    switch (type) {
        case TokenType::UNKNOWN:         return "UNKNOWN";
        case TokenType::END_OF_FILE:     return "END_OF_FILE";
        case TokenType::LEFT_BRACE:      return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:     return "RIGHT_BRACE";
        case TokenType::COLON:           return "COLON";
        case TokenType::SEMICOLON:       return "SEMICOLON";
        case TokenType::EQUAL:           return "EQUAL";
        case TokenType::IDENTIFIER:      return "IDENTIFIER";
        case TokenType::STRING:          return "STRING";
        case TokenType::KEYWORD_TEXT:    return "KEYWORD_TEXT";
        case TokenType::KEYWORD_STYLE:   return "KEYWORD_STYLE";
        case TokenType::NUMBER:          return "NUMBER";
        default:                         return "UNSPECIFIED";
    }
}

} // namespace CHTL
