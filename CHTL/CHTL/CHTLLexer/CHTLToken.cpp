#include "CHTLToken.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

std::string CHTLToken::toString() const {
    std::ostringstream oss;
    oss << "CHTLToken{type=" << static_cast<int>(type) 
        << ", value='" << value 
        << "', line=" << line 
        << ", column=" << column 
        << ", position=" << position << "}";
    return oss.str();
}

bool CHTLToken::isOperator() const {
    switch (type) {
        case CHTLTokenType::ASSIGN:
        case CHTLTokenType::EQUAL:
        case CHTLTokenType::NOT_EQUAL:
        case CHTLTokenType::LESS:
        case CHTLTokenType::GREATER:
        case CHTLTokenType::LESS_EQUAL:
        case CHTLTokenType::GREATER_EQUAL:
        case CHTLTokenType::PLUS:
        case CHTLTokenType::MINUS:
        case CHTLTokenType::MULTIPLY:
        case CHTLTokenType::DIVIDE:
        case CHTLTokenType::MODULO:
        case CHTLTokenType::POWER:
        case CHTLTokenType::AND:
        case CHTLTokenType::OR:
        case CHTLTokenType::NOT:
            return true;
        default:
            return false;
    }
}

bool CHTLToken::isKeyword() const {
    switch (type) {
        case CHTLTokenType::KEYWORD_INHERIT:
        case CHTLTokenType::KEYWORD_DELETE:
        case CHTLTokenType::KEYWORD_INSERT:
        case CHTLTokenType::KEYWORD_AFTER:
        case CHTLTokenType::KEYWORD_BEFORE:
        case CHTLTokenType::KEYWORD_REPLACE:
        case CHTLTokenType::KEYWORD_ATTOP:
        case CHTLTokenType::KEYWORD_ATBOTTOM:
        case CHTLTokenType::KEYWORD_FROM:
        case CHTLTokenType::KEYWORD_AS:
        case CHTLTokenType::KEYWORD_EXCEPT:
        case CHTLTokenType::KEYWORD_USE:
        case CHTLTokenType::KEYWORD_HTML5:
        case CHTLTokenType::KEYWORD_TEXT:
        case CHTLTokenType::KEYWORD_STYLE:
        case CHTLTokenType::KEYWORD_SCRIPT:
            return true;
        default:
            return false;
    }
}

bool CHTLToken::isLiteral() const {
    switch (type) {
        case CHTLTokenType::STRING:
        case CHTLTokenType::NUMBER:
        case CHTLTokenType::BOOLEAN:
            return true;
        default:
            return false;
    }
}

bool CHTLToken::isPunctuation() const {
    switch (type) {
        case CHTLTokenType::SEMICOLON:
        case CHTLTokenType::COMMA:
        case CHTLTokenType::DOT:
        case CHTLTokenType::LEFT_PAREN:
        case CHTLTokenType::RIGHT_PAREN:
        case CHTLTokenType::LEFT_BRACE:
        case CHTLTokenType::RIGHT_BRACE:
        case CHTLTokenType::LEFT_BRACKET:
        case CHTLTokenType::RIGHT_BRACKET:
        case CHTLTokenType::COLON:
        case CHTLTokenType::QUESTION:
        case CHTLTokenType::EXCLAMATION:
            return true;
        default:
            return false;
    }
}

bool CHTLToken::isHTML() const {
    return type == CHTLTokenType::HTML_ELEMENT;
}

void CHTLTokenList::addToken(const CHTLToken& token) {
    tokens.push_back(token);
}

void CHTLTokenList::addToken(CHTLTokenType type, const std::string& value, 
                            size_t line, size_t column, size_t position) {
    tokens.emplace_back(type, value, line, column, position);
}

CHTLToken CHTLTokenList::getCurrent() const {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

CHTLToken CHTLTokenList::getNext() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex++];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

CHTLToken CHTLTokenList::peek(size_t offset) const {
    size_t index = currentIndex + offset - 1;
    if (index < tokens.size()) {
        return tokens[index];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

bool CHTLTokenList::hasNext() const {
    return currentIndex < tokens.size();
}

bool CHTLTokenList::hasNext(size_t offset) const {
    return (currentIndex + offset - 1) < tokens.size();
}

void CHTLTokenList::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

void CHTLTokenList::reset() {
    currentIndex = 0;
}

size_t CHTLTokenList::size() const {
    return tokens.size();
}

bool CHTLTokenList::empty() const {
    return tokens.empty();
}

CHTLToken CHTLTokenList::operator[](size_t index) const {
    if (index < tokens.size()) {
        return tokens[index];
    }
    return CHTLToken(CHTLTokenType::EOF_TOKEN);
}

CHTLToken CHTLTokenList::at(size_t index) const {
    return operator[](index);
}

std::vector<CHTLToken> CHTLTokenList::getAll() const {
    return tokens;
}

std::vector<CHTLToken> CHTLTokenList::getRange(size_t start, size_t end) const {
    std::vector<CHTLToken> result;
    size_t actualEnd = std::min(end, tokens.size());
    for (size_t i = start; i < actualEnd; ++i) {
        result.push_back(tokens[i]);
    }
    return result;
}

void CHTLTokenList::clear() {
    tokens.clear();
    currentIndex = 0;
}

void CHTLTokenList::print() const {
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

} // namespace CHTL