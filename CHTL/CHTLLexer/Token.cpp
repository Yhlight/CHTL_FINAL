#include "Token.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

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
    return type_ == TokenType::COLON || 
           type_ == TokenType::EQUAL ||
           type_ == TokenType::SEMICOLON ||
           type_ == TokenType::COMMA ||
           type_ == TokenType::DOT ||
           type_ == TokenType::QUESTION ||
           type_ == TokenType::EXCLAMATION ||
           type_ == TokenType::ARROW ||
           type_ == TokenType::BIND_OP;
}

bool Token::isKeyword() const {
    return type_ == TokenType::USE ||
           type_ == TokenType::EXCEPT ||
           type_ == TokenType::DELETE ||
           type_ == TokenType::INSERT ||
           type_ == TokenType::INHERIT ||
           type_ == TokenType::FROM ||
           type_ == TokenType::AS ||
           type_ == TokenType::AT ||
           type_ == TokenType::TOP ||
           type_ == TokenType::BOTTOM ||
           type_ == TokenType::AFTER ||
           type_ == TokenType::BEFORE ||
           type_ == TokenType::REPLACE;
}

bool Token::isBracket() const {
    return type_ == TokenType::LEFT_PAREN ||
           type_ == TokenType::RIGHT_PAREN ||
           type_ == TokenType::LEFT_BRACE ||
           type_ == TokenType::RIGHT_BRACE ||
           type_ == TokenType::LEFT_BRACKET ||
           type_ == TokenType::RIGHT_BRACKET ||
           type_ == TokenType::LEFT_ANGLE ||
           type_ == TokenType::RIGHT_ANGLE;
}

bool Token::isHTML() const {
    return type_ == TokenType::HTML_ELEMENT;
}

bool Token::isTemplate() const {
    return type_ == TokenType::TEMPLATE ||
           type_ == TokenType::TEMPLATE_STYLE ||
           type_ == TokenType::TEMPLATE_ELEMENT ||
           type_ == TokenType::TEMPLATE_VAR;
}

bool Token::isCustom() const {
    return type_ == TokenType::CUSTOM ||
           type_ == TokenType::CUSTOM_STYLE ||
           type_ == TokenType::CUSTOM_ELEMENT ||
           type_ == TokenType::CUSTOM_VAR;
}

bool Token::isOrigin() const {
    return type_ == TokenType::ORIGIN ||
           type_ == TokenType::ORIGIN_HTML ||
           type_ == TokenType::ORIGIN_STYLE ||
           type_ == TokenType::ORIGIN_JAVASCRIPT;
}

bool Token::isImport() const {
    return type_ == TokenType::IMPORT ||
           type_ == TokenType::IMPORT_HTML ||
           type_ == TokenType::IMPORT_STYLE ||
           type_ == TokenType::IMPORT_JAVASCRIPT ||
           type_ == TokenType::IMPORT_CHTL ||
           type_ == TokenType::IMPORT_CJMOD;
}

bool Token::isNamespace() const {
    return type_ == TokenType::NAMESPACE;
}

bool Token::isConfiguration() const {
    return type_ == TokenType::CONFIGURATION ||
           type_ == TokenType::CONFIG_CONFIG;
}

} // namespace CHTL