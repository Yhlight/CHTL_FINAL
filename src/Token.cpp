#include "CHTL/Token.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

Token::Token(TokenType type, const std::string& value, size_t line, size_t column)
    : m_type(type)
    , m_value(value)
    , m_line(line)
    , m_column(column)
{
}

TokenType Token::getType() const {
    return m_type;
}

const std::string& Token::getValue() const {
    return m_value;
}

size_t Token::getLine() const {
    return m_line;
}

size_t Token::getColumn() const {
    return m_column;
}

void Token::setPosition(size_t line, size_t column) {
    m_line = line;
    m_column = column;
}

bool Token::isType(TokenType type) const {
    return m_type == type;
}

bool Token::isKeyword() const {
    return m_type >= TokenType::TEXT && m_type <= TokenType::HTML5;
}

bool Token::isSymbol() const {
    return m_type >= TokenType::LEFT_BRACE && m_type <= TokenType::BACKSLASH;
}

bool Token::isOperator() const {
    return m_type == TokenType::PLUS || m_type == TokenType::MINUS ||
           m_type == TokenType::ASTERISK || m_type == TokenType::SLASH ||
           m_type == TokenType::PERCENT || m_type == TokenType::EQUALS ||
           m_type == TokenType::LESS_THAN || m_type == TokenType::GREATER_THAN ||
           m_type == TokenType::AMPERSAND || m_type == TokenType::PIPE ||
           m_type == TokenType::EXCLAMATION || m_type == TokenType::TILDE ||
           m_type == TokenType::CARET;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << getTypeName() << ", \"" << m_value << "\", " 
        << m_line << ":" << m_column << ")";
    return oss.str();
}

std::string Token::getTypeName() const {
    switch (m_type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::DOLLAR: return "DOLLAR";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::EXCLAMATION: return "EXCLAMATION";
        case TokenType::TILDE: return "TILDE";
        case TokenType::CARET: return "CARET";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::BACKSLASH: return "BACKSLASH";
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
        case TokenType::USE: return "USE";
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
        case TokenType::HTML5: return "HTML5";
        case TokenType::VIR: return "VIR";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::ROUTER: return "ROUTER";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::FILELOADER: return "FILELOADER";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

TokenList::TokenList()
    : m_currentIndex(0)
{
}

void TokenList::addToken(const Token& token) {
    m_tokens.push_back(token);
}

void TokenList::addToken(TokenType type, const std::string& value, size_t line, size_t column) {
    m_tokens.emplace_back(type, value, line, column);
}

size_t TokenList::size() const {
    return m_tokens.size();
}

bool TokenList::empty() const {
    return m_tokens.empty();
}

const Token& TokenList::operator[](size_t index) const {
    return m_tokens[index];
}

Token& TokenList::operator[](size_t index) {
    return m_tokens[index];
}

const Token& TokenList::current() const {
    if (m_currentIndex < m_tokens.size()) {
        return m_tokens[m_currentIndex];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

const Token& TokenList::peek() const {
    if (m_currentIndex + 1 < m_tokens.size()) {
        return m_tokens[m_currentIndex + 1];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

bool TokenList::next() {
    if (m_currentIndex < m_tokens.size()) {
        m_currentIndex++;
        return true;
    }
    return false;
}

bool TokenList::isAtEnd() const {
    return m_currentIndex >= m_tokens.size();
}

void TokenList::reset() {
    m_currentIndex = 0;
}

void TokenList::clear() {
    m_tokens.clear();
    m_currentIndex = 0;
}

int TokenList::find(TokenType type, size_t startIndex) const {
    for (size_t i = startIndex; i < m_tokens.size(); ++i) {
        if (m_tokens[i].getType() == type) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int TokenList::find(const std::string& value, size_t startIndex) const {
    for (size_t i = startIndex; i < m_tokens.size(); ++i) {
        if (m_tokens[i].getValue() == value) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace CHTL