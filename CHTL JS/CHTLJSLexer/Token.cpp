#include "Token.h"
#include <sstream>

namespace CHTLJS {

Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : type(type), value(value), line(line), column(column) {
}

TokenType Token::getType() const {
    return type;
}

std::string Token::getValue() const {
    return value;
}

size_t Token::getLine() const {
    return line;
}

size_t Token::getColumn() const {
    return column;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << static_cast<int>(type) << ", \"" << value << "\", " 
        << line << ", " << column << ")";
    return oss.str();
}

bool Token::isOperator() const {
    return type == TokenType::ARROW || 
           type == TokenType::EVENT_ARROW ||
           type == TokenType::DOT ||
           type == TokenType::COMMA ||
           type == TokenType::COLON ||
           type == TokenType::SEMICOLON ||
           type == TokenType::EQUAL ||
           type == TokenType::QUESTION ||
           type == TokenType::EXCLAMATION ||
           type == TokenType::UNDERSCORE;
}

bool Token::isKeyword() const {
    return type == TokenType::VIR ||
           type == TokenType::LISTEN ||
           type == TokenType::ANIMATE ||
           type == TokenType::ROUTER ||
           type == TokenType::SCRIPT_LOADER ||
           type == TokenType::DELEGATE ||
           type == TokenType::INEVERAWAY ||
           type == TokenType::UTIL ||
           type == TokenType::CHANGE ||
           type == TokenType::THEN;
}

bool Token::isLiteral() const {
    return type == TokenType::STRING ||
           type == TokenType::NUMBER ||
           type == TokenType::BOOLEAN ||
           type == TokenType::IDENTIFIER;
}

bool Token::isPunctuation() const {
    return type == TokenType::LEFT_PAREN ||
           type == TokenType::RIGHT_PAREN ||
           type == TokenType::LEFT_BRACE ||
           type == TokenType::RIGHT_BRACE ||
           type == TokenType::LEFT_BRACKET ||
           type == TokenType::RIGHT_BRACKET ||
           type == TokenType::LEFT_ANGLE ||
           type == TokenType::RIGHT_ANGLE;
}

} // namespace CHTLJS