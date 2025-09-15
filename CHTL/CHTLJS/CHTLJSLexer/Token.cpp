#include "Token.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token{type=" << static_cast<int>(type) 
        << ", value='" << value 
        << "', line=" << line 
        << ", column=" << column 
        << ", position=" << position << "}";
    return oss.str();
}

bool Token::isOperator() const {
    switch (type) {
        case TokenType::ASSIGN:
        case TokenType::EQUAL:
        case TokenType::NOT_EQUAL:
        case TokenType::LESS:
        case TokenType::GREATER:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER_EQUAL:
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::MULTIPLY:
        case TokenType::DIVIDE:
        case TokenType::MODULO:
        case TokenType::POWER:
        case TokenType::AND:
        case TokenType::OR:
        case TokenType::NOT:
            return true;
        default:
            return false;
    }
}

bool Token::isKeyword() const {
    switch (type) {
        case TokenType::FUNCTION:
        case TokenType::VAR:
        case TokenType::LET:
        case TokenType::CONST:
        case TokenType::IF:
        case TokenType::ELSE:
        case TokenType::FOR:
        case TokenType::WHILE:
        case TokenType::DO:
        case TokenType::SWITCH:
        case TokenType::CASE:
        case TokenType::DEFAULT:
        case TokenType::BREAK:
        case TokenType::CONTINUE:
        case TokenType::RETURN:
        case TokenType::TRY:
        case TokenType::CATCH:
        case TokenType::FINALLY:
        case TokenType::THROW:
        case TokenType::NEW:
        case TokenType::THIS:
        case TokenType::SUPER:
        case TokenType::CLASS:
        case TokenType::EXTENDS:
        case TokenType::IMPLEMENTS:
        case TokenType::INTERFACE:
        case TokenType::ENUM:
        case TokenType::PUBLIC:
        case TokenType::PRIVATE:
        case TokenType::PROTECTED:
        case TokenType::STATIC:
        case TokenType::ABSTRACT:
        case TokenType::FINAL:
        case TokenType::NATIVE:
        case TokenType::SYNCHRONIZED:
        case TokenType::VOLATILE:
        case TokenType::TRANSIENT:
        case TokenType::STRICTFP:
        case TokenType::VIR:
        case TokenType::LISTEN:
        case TokenType::ANIMATE:
        case TokenType::ROUTER:
        case TokenType::SCRIPT_LOADER:
        case TokenType::DELEGATE:
            return true;
        default:
            return false;
    }
}

bool Token::isLiteral() const {
    switch (type) {
        case TokenType::STRING:
        case TokenType::NUMBER:
        case TokenType::BOOLEAN:
            return true;
        default:
            return false;
    }
}

bool Token::isPunctuation() const {
    switch (type) {
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
        case TokenType::DOT:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::COLON:
        case TokenType::QUESTION:
        case TokenType::EXCLAMATION:
            return true;
        default:
            return false;
    }
}

void TokenList::addToken(const Token& token) {
    tokens.push_back(token);
}

void TokenList::addToken(TokenType type, const std::string& value, 
                        size_t line, size_t column, size_t position) {
    tokens.emplace_back(type, value, line, column, position);
}

Token TokenList::getCurrent() const {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return Token(TokenType::EOF_TOKEN);
}

Token TokenList::getNext() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex++];
    }
    return Token(TokenType::EOF_TOKEN);
}

Token TokenList::peek(size_t offset) const {
    size_t index = currentIndex + offset - 1;
    if (index < tokens.size()) {
        return tokens[index];
    }
    return Token(TokenType::EOF_TOKEN);
}

bool TokenList::hasNext() const {
    return currentIndex < tokens.size();
}

bool TokenList::hasNext(size_t offset) const {
    return (currentIndex + offset - 1) < tokens.size();
}

void TokenList::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

void TokenList::reset() {
    currentIndex = 0;
}

size_t TokenList::size() const {
    return tokens.size();
}

bool TokenList::empty() const {
    return tokens.empty();
}

Token TokenList::operator[](size_t index) const {
    if (index < tokens.size()) {
        return tokens[index];
    }
    return Token(TokenType::EOF_TOKEN);
}

Token TokenList::at(size_t index) const {
    return operator[](index);
}

std::vector<Token> TokenList::getAll() const {
    return tokens;
}

std::vector<Token> TokenList::getRange(size_t start, size_t end) const {
    std::vector<Token> result;
    size_t actualEnd = std::min(end, tokens.size());
    for (size_t i = start; i < actualEnd; ++i) {
        result.push_back(tokens[i]);
    }
    return result;
}

void TokenList::clear() {
    tokens.clear();
    currentIndex = 0;
}

void TokenList::print() const {
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
}

} // namespace CHTLJS