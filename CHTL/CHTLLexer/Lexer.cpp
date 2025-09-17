#include "Lexer.h"
#include <map>

const std::map<std::string, TokenType> Lexer::keywords = {
    {"text", TokenType::TEXT},
    {"style", TokenType::STYLE},
    {"script", TokenType::SCRIPT},
    {"inherit", TokenType::INHERIT},
    {"delete", TokenType::DELETE},
    {"insert", TokenType::INSERT},
    {"after", TokenType::AFTER},
    {"before", TokenType::BEFORE},
    {"replace", TokenType::REPLACE},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"except", TokenType::EXCEPT},
    {"use", TokenType::USE},
    {"html5", TokenType::HTML5},
    // Special keywords require context, will be handled specially
    // "[Custom]" will be tokenized as LBRACKET, IDENTIFIER("Custom"), RBRACKET
};


Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, column - (int)text.length()});
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '@': addToken(TokenType::AT); break;
        case ':': addToken(TokenType::COLON); break;
        case '+': addToken(TokenType::PLUS); break;
        case '%': addToken(TokenType::PERCENT); break;

        case '-': addToken(match('>') ? TokenType::MINUS_GREATER : TokenType::MINUS); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;

        case '/':
            if (match('/')) {
                lineComment();
            } else if (match('*')) {
                blockComment();
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case '#':
            generatorComment();
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;

        case '\n':
            line++;
            column = 1;
            break;

        case '"':
        case '\'':
            stringLiteral();
            break;

        default:
            if (isDigit(c)) {
                numberLiteral();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                // For now, we'll just add an error token for unknown characters.
                // A more advanced implementation might handle parts of unquoted literals here.
                tokens.push_back({TokenType::ERROR, std::string(1, c), line, column - 1});
            }
            break;
    }
}

void Lexer::stringLiteral() {
    char quoteType = source[current - 1];
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        tokens.push_back({TokenType::ERROR, "Unterminated string.", line, column});
        return;
    }

    advance(); // The closing quote.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING);
}

void Lexer::numberLiteral() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        type = it->second;
    }
    addToken(type);
}

void Lexer::lineComment() {
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::blockComment() {
    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) {
        tokens.push_back({TokenType::ERROR, "Unterminated block comment.", line, column});
        return;
    }

    advance(); // Consume '*'
    advance(); // Consume '/'
}

void Lexer::generatorComment() {
    // # this is a generator comment
    if (peek() != ' ') {
        tokens.push_back({TokenType::ERROR, "# must be followed by a space for generator comments.", line, column});
        lineComment(); // Consume the rest of the line anyway
        return;
    }
    advance(); // consume space

    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
    addToken(TokenType::COMMENT);
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}
