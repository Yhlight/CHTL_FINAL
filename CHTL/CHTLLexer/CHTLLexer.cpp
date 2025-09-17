#include "CHTLLexer.h"
#include <iostream>
#include <unordered_map>

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
    return tokens;
}

Token CHTLLexer::nextToken() {
    skipWhitespaceAndComments();
    start = current;

    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c = advance();

    if (isalpha(c) || c == '_') return identifier();
    if (isdigit(c)) return number();

    switch (c) {
        case '{': return makeToken(TokenType::LBRACE);
        case '}': return makeToken(TokenType::RBRACE);
        case '(': return makeToken(TokenType::LPAREN);
        case ')': return makeToken(TokenType::RPAREN);
        case '[':
            if (match('T') && match('e') && match('m') && match('p') && match('l') && match('a') && match('t') && match('e') && match(']')) {
                return makeToken(TokenType::KEYWORD_TEMPLATE);
            }
            return makeToken(TokenType::LBRACKET);
        case ']': return makeToken(TokenType::RBRACKET);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '-': return makeToken(TokenType::MINUS);
        case '+': return makeToken(TokenType::PLUS);
        case '/': return makeToken(TokenType::SLASH);
        case '%': return makeToken(TokenType::PERCENT);
        case '?': return makeToken(TokenType::QUESTION_MARK);
        case '*':
            return makeToken(match('*') ? TokenType::DOUBLE_ASTERISK : TokenType::ASTERISK);
        case ':':
            return makeToken(TokenType::COLON);
        case '=':
            return makeToken(TokenType::EQUALS);
        case '&':
            return makeToken(match('&') ? TokenType::DOUBLE_AMPERSAND : TokenType::AMPERSAND);
        case '|':
            return makeToken(match('|') ? TokenType::DOUBLE_PIPE : TokenType::PIPE);
        case '"':
        case '\'':
            return stringLiteral();
        case '#':
            if (peek() == ' ') {
                while (peek() != '\n' && !isAtEnd()) advance();
                return makeToken(TokenType::GENERATOR_COMMENT);
            } else {
                return makeToken(TokenType::HASH);
            }
        case '@': {
            size_t at_start = current;
            while (isalpha(peek())) {
                advance();
            }
            std::string text = source.substr(at_start, current - at_start);
            if (text == "Style") return makeToken(TokenType::AT_STYLE);
            if (text == "Element") return makeToken(TokenType::AT_ELEMENT);
            if (text == "Var") return makeToken(TokenType::AT_VAR);
            if (text == "Html") return makeToken(TokenType::AT_HTML);
            return errorToken("Unknown @-rule.");
        }
    }

    return errorToken("Unexpected character.");
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
                line++;
                column = 1;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance();
                    advance();
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) advance();
                    if (!isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token CHTLLexer::stringLiteral() {
    char quoteType = source[current - 1];
    while (peek() != quoteType && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::STRING_LITERAL);
}

Token CHTLLexer::identifier() {
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;

    if (text == "text") type = TokenType::TEXT;
    else if (text == "style") type = TokenType::STYLE;
    else if (text == "script") type = TokenType::SCRIPT;
    else if (text == "inherit") type = TokenType::KEYWORD_INHERIT;

    return makeToken(type);
}

Token CHTLLexer::number() {
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }

    while (isalpha(peek()) || peek() == '%') advance();

    return makeToken(TokenType::UNQUOTED_LITERAL);
}

char CHTLLexer::advance() {
    current++;
    column++;
    return source[current - 1];
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

char CHTLLexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLLexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

Token CHTLLexer::makeToken(TokenType type) {
    return makeToken(type, source.substr(start, current - start));
}

Token CHTLLexer::makeToken(TokenType type, const std::string& lexeme) {
    Token token;
    token.type = type;
    token.lexeme = lexeme;
    token.line = line;
    token.column = column - (int)lexeme.length();
    return token;
}

Token CHTLLexer::errorToken(const std::string& message) {
    Token token;
    token.type = TokenType::ILLEGAL;
    token.lexeme = message;
    token.line = line;
    token.column = column;
    return token;
}
