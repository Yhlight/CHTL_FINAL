#include "Token.h"
#include <sstream>

namespace CHTLJS {

Token::Token(TokenType type, const std::string& value, int line, int column)
    : type_(type), value_(value), line_(line), column_(column) {
}

TokenType Token::getType() const {
    return type_;
}

const std::string& Token::getValue() const {
    return value_;
}

int Token::getLine() const {
    return line_;
}

int Token::getColumn() const {
    return column_;
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token(" << static_cast<int>(type_) << ", \"" << value_ << "\", " 
       << line_ << ", " << column_ << ")";
    return ss.str();
}

bool Token::isOperator() const {
    return type_ == TokenType::ARROW || 
           type_ == TokenType::BIND_OP ||
           type_ == TokenType::DOT ||
           type_ == TokenType::COMMA ||
           type_ == TokenType::SEMICOLON ||
           type_ == TokenType::COLON ||
           type_ == TokenType::EQUAL ||
           type_ == TokenType::QUESTION ||
           type_ == TokenType::EXCLAMATION;
}

bool Token::isKeyword() const {
    return type_ == TokenType::SCRIPT_LOADER ||
           type_ == TokenType::LISTEN ||
           type_ == TokenType::ANIMATE ||
           type_ == TokenType::ROUTER ||
           type_ == TokenType::VIR ||
           type_ == TokenType::INEVERAWAY ||
           type_ == TokenType::UTIL ||
           type_ == TokenType::PRINTMYLOVE;
}

bool Token::isBracket() const {
    return type_ == TokenType::LEFT_PAREN ||
           type_ == TokenType::RIGHT_PAREN ||
           type_ == TokenType::LEFT_BRACE ||
           type_ == TokenType::RIGHT_BRACE ||
           type_ == TokenType::LEFT_BRACKET ||
           type_ == TokenType::RIGHT_BRACKET;
}

} // namespace CHTLJS