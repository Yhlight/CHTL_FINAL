#include "CHTLLexer.h"
#include <iostream>
#include <unordered_map>

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::advance() {
    if (!isAtEnd()) {
        current++;
        column++;
    }
    return source[current - 1];
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

void CHTLLexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                advance();
                line++;
                column = 1;
                break;
            case '/':
                if (source[current + 1] == '/') {
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else if (source[current + 1] == '*') {
                    advance(); // Consume /
                    advance(); // Consume *
                    while (!(peek() == '*' && source[current + 1] == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) {
                        advance(); // Consume *
                        advance(); // Consume /
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token CHTLLexer::makeToken(TokenType type, const std::string& lexeme) {
    return {type, lexeme, line, column - (int)lexeme.length()};
}

Token CHTLLexer::makeToken(TokenType type) {
    return {type, std::string(1, source[current-1]), line, column - 1};
}

Token CHTLLexer::errorToken(const std::string& message) {
    return {TokenType::Unknown, message, line, column};
}

TokenType CHTLLexer::identifierType(const std::string& identifier) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"text", TokenType::Text},
        {"style", TokenType::Style},
    };
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return it->second;
    }
    return TokenType::Identifier;
}

Token CHTLLexer::identifier() {
    size_t start = current;
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    std::string lexeme = source.substr(start, current - start);
    return makeToken(identifierType(lexeme), lexeme);
}

Token CHTLLexer::unquotedLiteral() {
    size_t start = current;
    // Unquoted literals can contain almost anything until a semicolon, brace, or newline.
    // This is a simplification. The exact rules are complex.
    while (peek() != ';' && peek() != '{' && peek() != '}' && peek() != '\n' && !isAtEnd()) {
        advance();
    }
    std::string lexeme = source.substr(start, current - start);
    // Trim whitespace from the unquoted literal
    const std::string whitespace = " \t";
    size_t first = lexeme.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return makeToken(TokenType::UnquotedLiteral, "");
    }
    size_t last = lexeme.find_last_not_of(whitespace);
    lexeme = lexeme.substr(first, (last - first + 1));

    return makeToken(TokenType::UnquotedLiteral, lexeme);
}

Token CHTLLexer::stringLiteral() {
    char quote_type = advance(); // consume opening quote
    size_t start = current;
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string.");
    }

    std::string value = source.substr(start, current - start);
    advance(); // consume closing quote
    return makeToken(TokenType::StringLiteral, value);
}


std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        size_t start = current;
        char c = advance();

        if (isalpha(c) || c == '_') {
            current = start; // backtrack
            tokens.push_back(identifier());
            continue;
        }

        if (c == '"' || c == '\'') {
            current = start; // backtrack
            tokens.push_back(stringLiteral());
            continue;
        }

        switch (c) {
            case '{': tokens.push_back(makeToken(TokenType::LeftBrace)); break;
            case '}': tokens.push_back(makeToken(TokenType::RightBrace)); break;
            case '(': tokens.push_back(makeToken(TokenType::LeftParen)); break;
            case ')': tokens.push_back(makeToken(TokenType::RightParen)); break;
            case '[': tokens.push_back(makeToken(TokenType::LeftBracket)); break;
            case ']': tokens.push_back(makeToken(TokenType::RightBracket)); break;
            case ':': tokens.push_back(makeToken(TokenType::Colon)); break;
            case ';': tokens.push_back(makeToken(TokenType::Semicolon)); break;
            case '=': tokens.push_back(makeToken(TokenType::Equals)); break;
            case '@': tokens.push_back(makeToken(TokenType::At)); break;
            case '#': tokens.push_back(makeToken(TokenType::Hash)); break;
            case '.': tokens.push_back(makeToken(TokenType::Dot)); break;
            case ',': tokens.push_back(makeToken(TokenType::Comma)); break;
            case '&': tokens.push_back(makeToken(TokenType::Ampersand)); break;
            case '>': tokens.push_back(makeToken(TokenType::GreaterThan)); break;
            case '<': tokens.push_back(makeToken(TokenType::LessThan)); break;
            default:
                // If it's not a recognized symbol, it could be the start of an unquoted literal.
                // This is a very simplified assumption for now.
                current = start;
                tokens.push_back(unquotedLiteral());
                break;
        }
    }

    tokens.push_back({TokenType::EndOfFile, "", line, column});
    return tokens;
}
