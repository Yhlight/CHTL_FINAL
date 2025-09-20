#include "Token.h"

namespace CHTL {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE:  return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COLON:       return "COLON";
        case TokenType::SEMICOLON:   return "SEMICOLON";
        case TokenType::IDENTIFIER:  return "IDENTIFIER";
        case TokenType::STRING:      return "STRING";
        case TokenType::NUMBER:      return "NUMBER";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::UNKNOWN:     return "UNKNOWN";
        default:                     return "ERROR_UNKNOWN_TOKEN";
    }
}

} // namespace CHTL
