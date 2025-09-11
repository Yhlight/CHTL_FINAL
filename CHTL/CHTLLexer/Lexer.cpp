#include "Lexer.h"
#include <iostream>

Lexer::Lexer(const std::string& source, const ConfigurationContext& config)
    : source(source), config(config) {
    buildKeywordMap();
}

void Lexer::buildKeywordMap() {
    keyword_map.clear();
    for (const auto& kw : config.getKeyword("KEYWORD_DELETE")) keyword_map[kw] = TokenType::DeleteKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_INSERT")) keyword_map[kw] = TokenType::InsertKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_FROM")) keyword_map[kw] = TokenType::FromKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_USE")) keyword_map[kw] = TokenType::UseKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_EXCEPT")) keyword_map[kw] = TokenType::ExceptKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_TEMPLATE")) keyword_map[kw] = TokenType::TemplateKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_CUSTOM")) keyword_map[kw] = TokenType::CustomKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_IMPORT")) keyword_map[kw] = TokenType::ImportKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_CONFIGURATION")) keyword_map[kw] = TokenType::ConfigurationKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_NAMESPACE")) keyword_map[kw] = TokenType::NamespaceKeyword;
    for (const auto& kw : config.getKeyword("KEYWORD_EXPORT")) keyword_map[kw] = TokenType::ExportKeyword;
}

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
    if (isAtEnd()) return makeToken(TokenType::EndOfFile);

    char c = advance();
    if (isAlpha(c) || c == '_') return identifier();
    if (isDigit(c)) return identifier(); // Let parser handle numeric literals

    switch (c) {
        case '{': return makeToken(TokenType::OpenBrace);
        case '}': return makeToken(TokenType::CloseBrace);
        case '(': return makeToken(TokenType::OpenParen);
        case ')': return makeToken(TokenType::CloseParen);
        case '[':
            for (const auto& pair : keyword_map) {
                const std::string& kw = pair.first;
                if (kw.front() == '[' && source.compare(current - 1, kw.length(), kw) == 0) {
                    current += kw.length() - 1;
                    return makeToken(pair.second);
                }
            }
            return makeToken(TokenType::OpenBracket);
        case ']': return makeToken(TokenType::CloseBracket);
        case ':': return makeToken(TokenType::Colon);
        case ';': return makeToken(TokenType::Semicolon);
        case '=': return makeToken(TokenType::Equals);
        case '@': return makeToken(TokenType::At);
        case '#': return makeToken(TokenType::Hash);
        case '.': return makeToken(TokenType::Dot);
        case ',': return makeToken(TokenType::Comma);
        case '?': return makeToken(TokenType::QuestionMark);
        case '>': return makeToken(TokenType::GreaterThan);
        case '<': return makeToken(TokenType::LessThan);
        case '+': return makeToken(TokenType::Plus);
        case '%': return makeToken(TokenType::Percent);
        case '&': return (peek() == '&') ? (advance(), makeToken(TokenType::LogicalAnd)) : makeToken(TokenType::Ampersand);
        case '|': return (peek() == '|') ? (advance(), makeToken(TokenType::LogicalOr)) : errorToken("Unexpected character.");
        case '*': return (peek() == '*') ? (advance(), makeToken(TokenType::DoubleAsterisk)) : makeToken(TokenType::Asterisk);
        case '-':
            if (peek() == '-') {
                advance();
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
                    if (peek() == '\n') { line++; column = 0; }
                    advance();
                }
                if(isAtEnd()) return errorToken("Unterminated multi-line comment.");
                advance(); advance();
                return makeToken(TokenType::MultiLineComment);
            }
            return makeToken(TokenType::Slash);
        case '"': case '\'': return stringLiteral(c);
        default: return identifier();
    }
}

void Lexer::skipWhitespace() {
    while (true) {
        switch (peek()) {
            case ' ': case '\r': case '\t': advance(); break;
            case '\n': line++; column = 1; advance(); break;
            default: return;
        }
    }
}

Token Lexer::identifier() {
    while (isIdentifierChar(peek())) advance();
    std::string value = source.substr(start, current - start);
    auto it = keyword_map.find(value);
    if (it != keyword_map.end()) return makeToken(it->second, value);
    return makeToken(TokenType::Identifier, value);
}

Token Lexer::stringLiteral(char quote_type) {
    while (peek() != quote_type && !isAtEnd()) {
        if (peek() == '\n') { line++; column = 0; }
        advance();
    }
    if (isAtEnd()) return errorToken("Unterminated string.");
    advance();
    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::StringLiteral, value);
}

char Lexer::peek() { if (isAtEnd()) return '\0'; return source[current]; }
char Lexer::advance() { if (!isAtEnd()) { current++; column++; } return source[current - 1]; }
bool Lexer::isAtEnd() const { return current >= source.length(); }
bool Lexer::isAlpha(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool Lexer::isDigit(char c) const { return c >= '0' && c <= '9'; }
bool Lexer::isIdentifierChar(char c) const { return isAlpha(c) || isDigit(c) || c == '-'; }
Token Lexer::makeToken(TokenType type) const { return makeToken(type, source.substr(start, current - start)); }
Token Lexer::makeToken(TokenType type, const std::string& value) const { return Token{type, value, line, (int)(column - (current - start))}; }
Token Lexer::errorToken(const std::string& message) const { return Token{TokenType::Unexpected, message, line, column}; }
