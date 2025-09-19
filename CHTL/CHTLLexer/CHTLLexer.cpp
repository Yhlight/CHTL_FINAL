#include "CHTLLexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

namespace CHTL {

static std::unordered_map<std::string, TokenType> keywords = {
    {"style", TokenType::KEYWORD_STYLE},
    {"text", TokenType::KEYWORD_TEXT},
    {"script", TokenType::KEYWORD_SCRIPT},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at top", TokenType::KEYWORD_AT_TOP},
    {"at bottom", TokenType::KEYWORD_AT_BOTTOM},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"use", TokenType::KEYWORD_USE},
    {"html5", TokenType::KEYWORD_HTML5},
    {"Template", TokenType::KEYWORD_TEMPLATE},
    {"Custom", TokenType::KEYWORD_CUSTOM},
    {"Origin", TokenType::KEYWORD_ORIGIN},
    {"Import", TokenType::KEYWORD_IMPORT},
    {"Namespace", TokenType::KEYWORD_NAMESPACE},
    {"Configuration", TokenType::KEYWORD_CONFIGURATION},
    {"Info", TokenType::KEYWORD_INFO},
    {"Export", TokenType::KEYWORD_EXPORT}
};

CHTLLexer::CHTLLexer(const std::string& source) : source(source) {}

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
        case '[': scanBlockKeyword(); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case '@': addToken(TokenType::AT); break;
        case '.': addToken(TokenType::DOT); break;
        case ',': addToken(TokenType::COMMA); break;
        case ':': addToken(TokenType::COLON); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '?': addToken(TokenType::QUESTION); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(match('>') ? TokenType::ARROW : TokenType::MINUS); break;
        case '%': addToken(TokenType::PERCENT); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '&': addToken(match('&') ? TokenType::AMPERSAND_AMPERSAND : TokenType::AMPERSAND); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case '#': addToken(TokenType::HASH); break;
        case '$': addToken(TokenType::DOLLAR); break;

        case ' ': case '\r': case '\t': break;
        case '\n': line++; break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance();
                addToken(TokenType::COMMENT);
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') line++;
                    advance();
                }
                if (!isAtEnd()) { advance(); advance(); } // Consume the */
                addToken(TokenType::COMMENT);
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '"': case '\'': string(c); break;
        default:
            if (isdigit(c)) { number(); }
            else if (isalpha(c) || c == '_') { identifier(); }
            else { addToken(TokenType::UNKNOWN); }
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

    // Handle "at top" and "at bottom"
    if (text == "at" && (peek() == ' ' || peek() == '\t')) {
        int temp_current = current;
        while(peek() == ' ' || peek() == '\t') {
            advance();
        }
        int word_start = current;
        while (isalnum(peek()) || peek() == '_') advance();
        std::string next_word = source.substr(word_start, current - word_start);
        if(next_word == "top") {
            addToken(TokenType::KEYWORD_AT_TOP);
            return;
        } else if (next_word == "bottom") {
            addToken(TokenType::KEYWORD_AT_BOTTOM);
            return;
        }
        // backtrack
        current = temp_current;
    }

    auto it = keywords.find(text);
    addToken(it != keywords.end() ? it->second : TokenType::IDENTIFIER);
}

void CHTLLexer::scanBlockKeyword() {
    // Skip leading whitespace
    while(peek() == ' ' || peek() == '\t') {
        advance();
    }

    int keyword_start = current;
    while (isalpha(peek())) {
        advance();
    }
    std::string keyword = source.substr(keyword_start, current - keyword_start);

    // Skip trailing whitespace
    while(peek() == ' ' || peek() == '\t') {
        advance();
    }

    if (peek() == ']') {
        auto it = keywords.find(keyword);
        if (it != keywords.end() &&
            (it->second >= TokenType::KEYWORD_TEMPLATE && it->second <= TokenType::KEYWORD_EXPORT)) {
            advance(); // consume ']'
            addToken(it->second);
        } else {
            // Not a valid block keyword, backtrack and treat as individual tokens
            current = keyword_start;
            addToken(TokenType::LEFT_BRACKET);
        }
    } else {
        // Not a block keyword, backtrack and treat as individual tokens
        current = keyword_start;
        addToken(TokenType::LEFT_BRACKET);
    }
}

} // namespace CHTL
