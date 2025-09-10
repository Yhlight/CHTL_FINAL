#include "Lexer.h"
#include <iostream>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) {
        return makeToken(TokenType::EndOfFile);
    }

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) {
        // For now, treat numbers as part of an identifier, e.g., "100px"
        // The parser will handle validation.
        return identifier();
    }

    switch (c) {
        case '{': return makeToken(TokenType::OpenBrace);
        case '}': return makeToken(TokenType::CloseBrace);
        case '(': return makeToken(TokenType::OpenParen);
        case ')': return makeToken(TokenType::CloseParen);
        case '[': return makeToken(TokenType::OpenBracket);
        case ']': return makeToken(TokenType::CloseBracket);
        case ':': return makeToken(TokenType::Colon);
        case ';': return makeToken(TokenType::Semicolon);
        case '=': return makeToken(TokenType::Equals);
        case '@': return makeToken(TokenType::At);
        case '#': return makeToken(TokenType::Hash);
        case '.': return makeToken(TokenType::Dot);
        case ',': return makeToken(TokenType::Comma);
        case '?': return makeToken(TokenType::QuestionMark);

        case '+': return makeToken(TokenType::Plus);
        case '%': return makeToken(TokenType::Percent);

        case '&':
            return (peek() == '&') ? (advance(), makeToken(TokenType::LogicalAnd)) : makeToken(TokenType::Ampersand);
        case '|':
            return (peek() == '|') ? (advance(), makeToken(TokenType::LogicalOr)) : errorToken("Unexpected character.");

        case '*':
            return (peek() == '*') ? (advance(), makeToken(TokenType::DoubleAsterisk)) : makeToken(TokenType::Asterisk);

        case '-':
            if (peek() == '-') {
                advance();
                // It's a generator comment, but we'll treat it as a comment to be skipped for now.
                // The parser can decide what to do with it. For now, we'll just create a token.
                while(peek() != '\n' && !isAtEnd()) advance();
                return makeToken(TokenType::GeneratorComment);
            }
            return makeToken(TokenType::Minus);

        case '/':
            if (peek() == '/') {
                advance();
                while (peek() != '\n' && !isAtEnd()) advance();
                return makeToken(TokenType::SingleLineComment);
            } else if (peek() == '*') {
                advance();
                while (!(peek() == '*' && source[current+1] == '/') && !isAtEnd()) {
                    if (peek() == '\n') {
                        line++;
                        column = 0;
                    }
                    advance();
                }
                if(isAtEnd()) return errorToken("Unterminated multi-line comment.");
                advance(); // consume '*'
                advance(); // consume '/'
                return makeToken(TokenType::MultiLineComment);
            }
            return makeToken(TokenType::Slash);

        case '"':
        case '\'':
            return stringLiteral(c);

        default:
            // For unquoted literals that might not start with a letter
            if (!isspace(c)) return identifier();
            return errorToken("Unexpected character.");
    }
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                column = 1;
                advance();
                break;
            default:
                return;
        }
    }
}

Token Lexer::identifier() {
    while (isIdentifierChar(peek())) {
        advance();
    }
    std::string value = source.substr(start, current - start);
    // Here we could check for keywords, but CHTL has context-sensitive keywords.
    // e.g. `text` is a keyword for a node type, but can be part of a value.
    // The parser is a better place to handle this.
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::stringLiteral(char quote_type) {
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 0;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    advance(); // The closing quote.

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::StringLiteral, value);
}


char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::advance() {
    if (!isAtEnd()) {
        current++;
        column++;
    }
    return source[current - 1];
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isIdentifierChar(char c) const {
    // This is a broad definition to support things like `100px`, `my-class`, etc.
    // It allows anything that is not a whitespace or a special symbol.
    // We allow '-' in identifiers.
    return c != '\0' && !isspace(c) &&
           std::string("}{[]():;=@#.,+*/%&|?").find(c) == std::string::npos;
}


Token Lexer::makeToken(TokenType type) const {
    return makeToken(type, source.substr(start, current - start));
}

Token Lexer::makeToken(TokenType type, const std::string& value) const {
    return Token{type, value, line, (int)(column - (current - start))};
}

Token Lexer::errorToken(const std::string& message) const {
    return Token{TokenType::Unexpected, message, line, column};
}
