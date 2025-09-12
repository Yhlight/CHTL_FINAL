#include "CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        start_column = column;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool CHTLLexer::isAtEnd() const {
    return current >= source.length();
}

char CHTLLexer::advance() {
    if (!isAtEnd()) {
        column++;
        return source[current++];
    }
    return '\0';
}

void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, line, start_column);
}

void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    tokens.emplace_back(type, literal, line, start_column);
}

char CHTLLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    column++;
    current++;
    return true;
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '.': addToken(TokenType::DOT); break;
        case '#': addToken(TokenType::HASH); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '+': addToken(TokenType::PLUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*':
            if (match('*')) {
                addToken(TokenType::STAR_STAR);
            } else {
                addToken(TokenType::STAR);
            }
            break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '"':
        case '\'':
            handleString();
            break;

        case '/':
            if (match('/')) { // Line comment
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) { // Block comment
                handleBlockComment();
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case '-':
            if (match('-')) { // CHTL Comment
                handleChtlComment();
            } else {
                addToken(TokenType::MINUS);
            }
            break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            column = 1;
            break;

        default:
            if (std::isdigit(c)) {
                handleNumber();
            } else if (std::isalpha(c) || c == '_') {
                handleIdentifier();
            } else if (!isAtEnd()) {
                // Ignore other characters
            }
            break;
    }
}

void CHTLLexer::handleIdentifier() {
    while (std::isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }
    std::string text = source.substr(start, current - start);
    TokenType type = globalMap.getKeywordType(text);
    if (type == TokenType::IDENTIFIER) {
        addToken(TokenType::UNQUOTED_LITERAL, text);
    } else {
        addToken(type);
    }
}

void CHTLLexer::handleString() {
    char quote_type = source[current - 1];
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }
    if (isAtEnd()) return; // Unterminated string
    advance(); // The closing quote
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLLexer::handleBlockComment() {
    while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }
    if (!isAtEnd()) {
        advance(); // consume '*'
        advance(); // consume '/'
    }
}

void CHTLLexer::handleChtlComment() {
    int comment_start = current;
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    std::string comment_text = source.substr(comment_start, current - comment_start);
    addToken(TokenType::CHTL_COMMENT, comment_text);
}

void CHTLLexer::handleNumber() {
    while (std::isdigit(peek())) {
        advance();
    }
    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peekNext())) {
        // Consume the "."
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }
    // For now, we tokenize the whole thing as a number, including units like 'px'
    // A more advanced lexer/parser would handle units separately.
    while (std::isalpha(peek())) {
        advance();
    }
    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

} // namespace CHTL
