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
    if (isdigit(c)) return number(); // For now, let's just consume a simple number

    switch (c) {
        case '{': return makeToken(TokenType::LBRACE);
        case '}': return makeToken(TokenType::RBRACE);
        case '(': return makeToken(TokenType::LPAREN);
        case ')': return makeToken(TokenType::RPAREN);
        case '[': return makeToken(TokenType::LBRACKET);
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
            // If it's followed by a space, it's a generator comment.
            // Otherwise, it's a HASH for an ID selector.
            if (peek() == ' ') {
                while (peek() != '\n' && !isAtEnd()) advance();
                return makeToken(TokenType::GENERATOR_COMMENT);
            } else {
                return makeToken(TokenType::HASH);
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
                    advance(); // consume /
                    advance(); // consume *
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!isAtEnd()) advance(); // consume *
                    if (!isAtEnd()) advance(); // consume /
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

    advance(); // The closing quote.
    return makeToken(TokenType::STRING_LITERAL);
}

Token CHTLLexer::identifier() {
    // Allow hyphens in identifiers for CSS properties like font-weight
    while (isalnum(peek()) || peek() == '_' || peek() == '-') advance();

    // Check if it's a keyword
    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;

    // This is a simple but inefficient way to check for keywords.
    // A trie or a perfect hash function would be better for a production compiler.
    if (text == "text") type = TokenType::TEXT;
    else if (text == "style") type = TokenType::STYLE;
    else if (text == "script") type = TokenType::SCRIPT;
    else if (text == "inherit") type = TokenType::KEYWORD_INHERIT;
    // ... add all other keywords here

    // For now, let's treat unquoted literals as identifiers as well.
    // The parser will decide how to interpret them based on context.
    // A more sophisticated lexer might distinguish them.
    // For example, `color: red` -> IDENTIFIER("color"), COLON, IDENTIFIER("red"), SEMICOLON
    // `width: 100px` -> IDENTIFIER("width"), COLON, IDENTIFIER("100px"), SEMICOLON
    // Let's create a separate type for things that look like unquoted literals
    // if the parser needs it. For now, IDENTIFIER is fine.

    return makeToken(type);
}


Token CHTLLexer::number() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the ".".
        advance();
        while (isdigit(peek())) advance();
    }

    // Also consume any unit suffix like 'px', 'em', '%' etc.
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
