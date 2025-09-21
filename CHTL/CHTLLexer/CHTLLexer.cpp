#include "CHTLLexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

namespace CHTL {

// Helper to map the internal config key names to the actual TokenType enum
static const std::map<std::string, TokenType> internal_keyword_to_token_type = {
    {"KEYWORD_STYLE", TokenType::STYLE},
    {"KEYWORD_SCRIPT", TokenType::SCRIPT},
    {"KEYWORD_TEXT", TokenType::TEXT},
    {"KEYWORD_USE", TokenType::USE},
    {"KEYWORD_INHERIT", TokenType::INHERIT},
    {"KEYWORD_FROM", TokenType::FROM},
    {"KEYWORD_AS", TokenType::AS},
    {"KEYWORD_DELETE", TokenType::DELETE},
    {"KEYWORD_INSERT", TokenType::INSERT},
    {"KEYWORD_AFTER", TokenType::AFTER},
    {"KEYWORD_BEFORE", TokenType::BEFORE},
    {"KEYWORD_REPLACE", TokenType::REPLACE},
    {"KEYWORD_EXCEPT", TokenType::EXCEPT},
    // Note: Bracketed keywords like [Custom] are handled by the parser, not the lexer.
};


CHTLLexer::CHTLLexer(const std::string& source, std::shared_ptr<Configuration> config)
    : source(source), config(config) {
    // Build the runtime keyword map from the configuration
    for (const auto& pair : config->keyword_map) {
        if (internal_keyword_to_token_type.count(pair.first)) {
            TokenType token_type = internal_keyword_to_token_type.at(pair.first);
            for (const std::string& lexeme : pair.second) {
                this->runtime_keyword_map[lexeme] = token_type;
            }
        }
    }
}

std::vector<Token> CHTLLexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back({TokenType::END_OF_FILE, "", line, start});
    return tokens;
}

bool CHTLLexer::isAtEnd() {
    return current >= source.length();
}

void CHTLLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(match('{') ? TokenType::LEFT_BRACE_BRACE : TokenType::LEFT_BRACE); break;
        case '}': addToken(match('}') ? TokenType::RIGHT_BRACE_BRACE : TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case '@': addToken(TokenType::AT); break;
        case '.': addToken(TokenType::DOT); break;
        case ',': addToken(TokenType::COMMA); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) { advance(); advance(); } // Consume the */
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '"': case '\'': string(c); break;
        default:
            if (isdigit(c)) { number(); }
            else if (isalpha(c) || c == '_') { identifier(); }
            else { addToken(TokenType::SYMBOL); }
            break;
    }
}

char CHTLLexer::advance() { return source[current++]; }
void CHTLLexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line, start});
}
void CHTLLexer::addToken(TokenType type, const std::string& literal) {
    tokens.push_back({type, literal, line, start});
}
bool CHTLLexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}
char CHTLLexer::peek() { return isAtEnd() ? '\0' : source[current]; }
char CHTLLexer::peekNext() { return current + 1 >= source.length() ? '\0' : source[current + 1]; }

void CHTLLexer::string(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) { return; } // Unterminated string
    advance(); // The closing quote.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void CHTLLexer::number() {
    while (isdigit(peek())) advance();
    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        while (isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

void CHTLLexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    auto it = runtime_keyword_map.find(text);
    addToken(it != runtime_keyword_map.end() ? it->second : TokenType::IDENTIFIER);
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::AT: return "AT";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::DOT: return "DOT";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::COMMA: return "COMMA";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::AMPERSAND_AMPERSAND: return "AMPERSAND_AMPERSAND";
        case TokenType::PIPE: return "PIPE";
        case TokenType::PIPE_PIPE: return "PIPE_PIPE";
        case TokenType::STAR_STAR: return "STAR_STAR";
        case TokenType::LEFT_BRACE_BRACE: return "LEFT_BRACE_BRACE";
        case TokenType::RIGHT_BRACE_BRACE: return "RIGHT_BRACE_BRACE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STYLE: return "STYLE";
        case TokenType::TEXT: return "TEXT";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::USE: return "USE";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN_TOKEN";
    }
}

} // namespace CHTL
