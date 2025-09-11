#include "Lexer.h"
#include <iostream>

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

char Lexer::advance() {
    current++;
    return source[current - 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literalValue) {
    std::string text = source.substr(start, current - start);
    // For tokens where the literal value is different from the lexeme
    // (like strings), we pass it in. Otherwise, the lexeme is the value.
    tokens.push_back({type, text, line});
}

void Lexer::scanString(char quoteType) {
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        // Unterminated string. Report error in a real compiler.
    }

    advance(); // The closing quote.
    addToken(TokenType::STRING);
}

void Lexer::scanNumber() {
    while (isdigit(peek())) {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) {
            advance();
        }
    }

    addToken(TokenType::NUMBER);
}

void Lexer::scanIdentifier() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') {
        advance();
    }

    std::string text = source.substr(start, current - start);
    TokenType type;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    } else {
        type = TokenType::IDENTIFIER;
    }
    addToken(type);
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;

        case '[': {
            if (match('T') && match('e') && match('m') && match('p') && match('l') && match('a') && match('t') && match('e') && match(']')) {
                addToken(TokenType::KEYWORD_TEMPLATE);
            } else if (match('C') && match('u') && match('s') && match('t') && match('o') && match('m') && match(']')) {
                addToken(TokenType::KEYWORD_CUSTOM);
            } else {
                // It was a false alarm, just a normal bracket.
                // We need to reset the current pointer because match() advanced it.
                current = start + 1;
                addToken(TokenType::LEFT_BRACKET);
            }
            break;
        }
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '@': addToken(TokenType::AT); break;
        case '=': addToken(TokenType::EQUAL); break;
        case '+': addToken(TokenType::PLUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '>': addToken(TokenType::GREATER); break;
        case '<': addToken(TokenType::LESS); break;
        case '&': addToken(TokenType::AMPERSAND); break;
        case '.': addToken(TokenType::DOT); break;
        case '#': addToken(TokenType::HASH); break;
        case '?': addToken(TokenType::QUESTION); break;

        case '*':
            addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR);
            break;

        case '-':
            if (match('-')) {
                // Generator comment. For the lexer, we create a token
                // for the '--' and then tokenize the rest of the line normally.
                // The parser will decide how to handle this.
                tokens.push_back({TokenType::GEN_COMMENT, "--", line});
                start = current; // Reset start for the next token.
            } else {
                addToken(TokenType::MINUS);
            }
            break;

        case '/':
            if (match('/')) {
                // A single-line comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                // A block comment
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume *
                    advance(); // consume /
                }
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        // Ignore whitespace.
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        case '"':
        case '\'':
            scanString(c);
            break;

        default:
            if (isalpha(c) || c == '_') {
                scanIdentifier();
            } else if (isdigit(c)) {
                scanNumber();
            } else {
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}
