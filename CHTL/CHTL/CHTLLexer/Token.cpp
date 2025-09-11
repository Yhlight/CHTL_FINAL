#include "Token.h"
#include <sstream>
#include <map>

namespace CHTL {

bool Token::isKeyword() const {
    static const std::map<TokenType, bool> keywordTypes = {
        {TokenType::TEXT, true},
        {TokenType::STYLE, true},
        {TokenType::SCRIPT, true},
        {TokenType::TEMPLATE, true},
        {TokenType::CUSTOM, true},
        {TokenType::ORIGIN, true},
        {TokenType::IMPORT, true},
        {TokenType::NAMESPACE, true},
        {TokenType::CONFIGURATION, true},
        {TokenType::USE, true},
        {TokenType::INHERIT, true},
        {TokenType::DELETE, true},
        {TokenType::INSERT, true},
        {TokenType::EXCEPT, true},
        {TokenType::FROM, true},
        {TokenType::AS, true},
        {TokenType::AT_TOP, true},
        {TokenType::AT_BOTTOM, true},
        {TokenType::AFTER, true},
        {TokenType::BEFORE, true},
        {TokenType::REPLACE, true},
        {TokenType::HTML5, true}
    };
    
    return keywordTypes.count(type) > 0;
}

bool Token::isOperator() const {
    static const std::map<TokenType, bool> operatorTypes = {
        {TokenType::PLUS, true},
        {TokenType::MINUS, true},
        {TokenType::ASTERISK, true},
        {TokenType::SLASH, true},
        {TokenType::PERCENT, true},
        {TokenType::CARET, true},
        {TokenType::EQUAL, true},
        {TokenType::EQUAL_EQUAL, true},
        {TokenType::NOT_EQUAL, true},
        {TokenType::LESS, true},
        {TokenType::GREATER, true},
        {TokenType::LESS_EQUAL, true},
        {TokenType::GREATER_EQUAL, true},
        {TokenType::AND, true},
        {TokenType::OR, true},
        {TokenType::ARROW, true},
        {TokenType::DOUBLE_ARROW, true},
        {TokenType::POWER, true},
        {TokenType::INCREMENT, true},
        {TokenType::DECREMENT, true}
    };
    
    return operatorTypes.count(type) > 0;
}

bool Token::isDelimiter() const {
    static const std::map<TokenType, bool> delimiterTypes = {
        {TokenType::LEFT_BRACE, true},
        {TokenType::RIGHT_BRACE, true},
        {TokenType::LEFT_PAREN, true},
        {TokenType::RIGHT_PAREN, true},
        {TokenType::LEFT_BRACKET, true},
        {TokenType::RIGHT_BRACKET, true},
        {TokenType::SEMICOLON, true},
        {TokenType::COLON, true},
        {TokenType::COMMA, true},
        {TokenType::DOT, true}
    };
    
    return delimiterTypes.count(type) > 0;
}

std::string Token::getTypeName() const {
    static const std::map<TokenType, std::string> typeNames = {
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::LITERAL, "LITERAL"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COLON, "COLON"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::HASH, "HASH"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::EXCLAMATION, "EXCLAMATION"},
        {TokenType::PIPE, "PIPE"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::BACKSLASH, "BACKSLASH"},
        {TokenType::ASTERISK, "ASTERISK"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::CARET, "CARET"},
        {TokenType::TILDE, "TILDE"},
        {TokenType::AT, "AT"},
        {TokenType::DOLLAR, "DOLLAR"},
        {TokenType::UNDERSCORE, "UNDERSCORE"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::ARROW, "ARROW"},
        {TokenType::DOUBLE_ARROW, "DOUBLE_ARROW"},
        {TokenType::POWER, "POWER"},
        {TokenType::INCREMENT, "INCREMENT"},
        {TokenType::DECREMENT, "DECREMENT"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::SINGLE_COMMENT, "SINGLE_COMMENT"},
        {TokenType::MULTI_COMMENT, "MULTI_COMMENT"},
        {TokenType::GENERATOR_COMMENT, "GENERATOR_COMMENT"},
        {TokenType::TEXT, "TEXT"},
        {TokenType::STYLE, "STYLE"},
        {TokenType::SCRIPT, "SCRIPT"},
        {TokenType::TEMPLATE, "TEMPLATE"},
        {TokenType::CUSTOM, "CUSTOM"},
        {TokenType::ORIGIN, "ORIGIN"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::NAMESPACE, "NAMESPACE"},
        {TokenType::CONFIGURATION, "CONFIGURATION"},
        {TokenType::USE, "USE"},
        {TokenType::INHERIT, "INHERIT"},
        {TokenType::DELETE, "DELETE"},
        {TokenType::INSERT, "INSERT"},
        {TokenType::EXCEPT, "EXCEPT"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::AT_TOP, "AT_TOP"},
        {TokenType::AT_BOTTOM, "AT_BOTTOM"},
        {TokenType::AFTER, "AFTER"},
        {TokenType::BEFORE, "BEFORE"},
        {TokenType::REPLACE, "REPLACE"},
        {TokenType::HTML5, "HTML5"},
        {TokenType::RESPONSIVE_VALUE, "RESPONSIVE_VALUE"},
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::WHITESPACE, "WHITESPACE"},
        {TokenType::UNKNOWN, "UNKNOWN"}
    };
    
    auto it = typeNames.find(type);
    return (it != typeNames.end()) ? it->second : "UNKNOWN";
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << getTypeName() << ", \"" << value << "\", " 
        << line << ":" << column << ")";
    return oss.str();
}

bool Token::operator==(const Token& other) const {
    return type == other.type && value == other.value;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

// TokenStream 实现
void TokenStream::addToken(const Token& token) {
    tokens.push_back(token);
}

void TokenStream::addTokens(const std::vector<Token>& newTokens) {
    tokens.insert(tokens.end(), newTokens.begin(), newTokens.end());
}

Token TokenStream::peek(size_t offset) const {
    size_t index = currentIndex + offset;
    if (index >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
    }
    return tokens[index];
}

Token TokenStream::consume() {
    if (currentIndex >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
    }
    return tokens[currentIndex++];
}

bool TokenStream::hasNext() const {
    return currentIndex < tokens.size();
}

size_t TokenStream::size() const {
    return tokens.size();
}

size_t TokenStream::getCurrentIndex() const {
    return currentIndex;
}

void TokenStream::reset() {
    currentIndex = 0;
}

Token TokenStream::findNext(TokenType type) const {
    for (size_t i = currentIndex; i < tokens.size(); ++i) {
        if (tokens[i].type == type) {
            return tokens[i];
        }
    }
    return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
}

bool TokenStream::hasNext(TokenType type) const {
    return findNext(type).type != TokenType::END_OF_FILE;
}

void TokenStream::skipWhitespaceAndComments() {
    while (hasNext()) {
        Token token = peek();
        if (token.type == TokenType::WHITESPACE || 
            token.type == TokenType::NEWLINE ||
            token.type == TokenType::SINGLE_COMMENT ||
            token.type == TokenType::MULTI_COMMENT ||
            token.type == TokenType::GENERATOR_COMMENT) {
            consume();
        } else {
            break;
        }
    }
}

Token TokenStream::current() const {
    if (currentIndex == 0 || currentIndex > tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0, 0);
    }
    return tokens[currentIndex - 1];
}

void TokenStream::backtrack() {
    if (currentIndex > 0) {
        currentIndex--;
    }
}

} // namespace CHTL