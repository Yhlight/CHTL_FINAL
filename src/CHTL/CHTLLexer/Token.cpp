#include "Token.h"
#include <sstream>

namespace CHTL {

Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : m_type(type), m_value(value), m_line(line), m_column(column) {}

bool Token::isKeyword() const {
    switch (m_type) {
        case TokenType::TEXT:
        case TokenType::STYLE:
        case TokenType::SCRIPT:
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
        case TokenType::ORIGIN:
        case TokenType::IMPORT:
        case TokenType::NAMESPACE:
        case TokenType::CONFIGURATION:
        case TokenType::INFO:
        case TokenType::EXPORT:
        case TokenType::AT_STYLE:
        case TokenType::AT_ELEMENT:
        case TokenType::AT_VAR:
        case TokenType::AT_HTML:
        case TokenType::AT_JAVASCRIPT:
        case TokenType::AT_CHTL:
        case TokenType::AT_CONFIG:
        case TokenType::DELETE:
        case TokenType::INSERT:
        case TokenType::INHERIT:
        case TokenType::EXCEPT:
        case TokenType::USE:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::AFTER:
        case TokenType::BEFORE:
        case TokenType::REPLACE:
        case TokenType::AT_TOP:
        case TokenType::AT_BOTTOM:
        case TokenType::VIR:
        case TokenType::LISTEN:
        case TokenType::ANIMATE:
        case TokenType::ROUTER:
        case TokenType::FILELOADER:
        case TokenType::DELEGATE:
        case TokenType::INEVERAWAY:
        case TokenType::UTIL:
        case TokenType::CHANGE:
        case TokenType::THEN:
        case TokenType::PRINTMYLOVE:
            return true;
        default:
            return false;
    }
}

bool Token::isOperator() const {
    switch (m_type) {
        case TokenType::QUESTION:
        case TokenType::LOGICAL_AND:
        case TokenType::LOGICAL_OR:
        case TokenType::GREATER:
        case TokenType::LESS:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS_EQUAL:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
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
    switch (m_type) {
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::DOT:
        case TokenType::COLON:
        case TokenType::EQUALS:
            return true;
        default:
            return false;
    }
}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token(" << typeToString(m_type) << ", \"" << m_value << "\", " 
       << m_line << ":" << m_column << ")";
    return ss.str();
}

std::string Token::typeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LITERAL: return "LITERAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::SINGLE_COMMENT: return "SINGLE_COMMENT";
        case TokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::USE: return "USE";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::HASH: return "HASH";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
        case TokenType::VIR: return "VIR";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::ROUTER: return "ROUTER";
        case TokenType::FILELOADER: return "FILELOADER";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::INEVERAWAY: return "INEVERAWAY";
        case TokenType::UTIL: return "UTIL";
        case TokenType::CHANGE: return "CHANGE";
        case TokenType::THEN: return "THEN";
        case TokenType::PRINTMYLOVE: return "PRINTMYLOVE";
        default: return "UNKNOWN";
    }
}

// TokenStream implementation
TokenStream::TokenStream() : m_position(0) {}

void TokenStream::addToken(std::unique_ptr<Token> token) {
    m_tokens.push_back(std::move(token));
}

Token* TokenStream::current() const {
    if (m_position >= m_tokens.size()) {
        return nullptr;
    }
    return m_tokens[m_position].get();
}

Token* TokenStream::next() {
    if (m_position >= m_tokens.size()) {
        return nullptr;
    }
    return m_tokens[m_position++].get();
}

Token* TokenStream::at(size_t index) const {
    if (index >= m_tokens.size()) {
        return nullptr;
    }
    return m_tokens[index].get();
}

bool TokenStream::hasMore() const {
    return m_position < m_tokens.size();
}

size_t TokenStream::size() const {
    return m_tokens.size();
}

size_t TokenStream::getPosition() const {
    return m_position;
}

void TokenStream::setPosition(size_t position) {
    if (position <= m_tokens.size()) {
        m_position = position;
    }
}

void TokenStream::reset() {
    m_position = 0;
}

void TokenStream::clear() {
    m_tokens.clear();
    m_position = 0;
}

bool TokenStream::empty() const {
    return m_tokens.empty();
}

} // namespace CHTL