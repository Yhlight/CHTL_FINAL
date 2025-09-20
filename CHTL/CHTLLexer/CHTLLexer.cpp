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
    {"KEYWORD_HTML5", TokenType::HTML5},
    {"KEYWORD_INHERIT", TokenType::INHERIT},
    {"KEYWORD_FROM", TokenType::FROM},
    {"KEYWORD_AS", TokenType::AS},
    {"KEYWORD_DELETE", TokenType::DELETE},
    {"KEYWORD_INSERT", TokenType::INSERT},
    {"KEYWORD_AFTER", TokenType::AFTER},
    {"KEYWORD_BEFORE", TokenType::BEFORE},
    {"KEYWORD_REPLACE", TokenType::REPLACE},
    {"KEYWORD_ATTOP", TokenType::AT_TOP},
    {"KEYWORD_ATBOTTOM", TokenType::AT_BOTTOM},
    {"KEYWORD_EXCEPT", TokenType::EXCEPT},
    {"KEYWORD_CUSTOM", TokenType::CUSTOM},
    {"KEYWORD_TEMPLATE", TokenType::TEMPLATE},
    {"KEYWORD_ORIGIN", TokenType::ORIGIN},
    {"KEYWORD_IMPORT", TokenType::IMPORT},
    {"KEYWORD_NAMESPACE", TokenType::NAMESPACE},
    {"KEYWORD_CONFIGURATION", TokenType::CONFIGURATION},
    {"NAME_GROUP", TokenType::NAME_GROUP},
    {"ORIGIN_TYPE_GROUP", TokenType::ORIGIN_TYPE_GROUP},
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
        case '%': addToken(TokenType::PERCENT); break;
        case '*': addToken(match('*') ? TokenType::STAR_STAR : TokenType::STAR); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        case '|': addToken(match('|') ? TokenType::PIPE_PIPE : TokenType::PIPE); break;
        case '$': addToken(TokenType::DOLLAR); break;
        case '#':
            while (peek() != '\n' && !isAtEnd()) advance();
            addToken(TokenType::COMMENT);
            break;
        case '-':
            if (match('>')) addToken(TokenType::ARROW);
            else addToken(TokenType::MINUS);
            break;
        case '&':
            if (match('-') && match('>')) addToken(TokenType::EVENT_BIND);
            else if (match('&')) addToken(TokenType::AMPERSAND_AMPERSAND);
            else addToken(TokenType::AMPERSAND);
            break;
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
    addToken(type, source.substr(start, current - start));
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

    // Check for multi-word keywords
    if (isspace(peek())) {
        int temp_current = current;
        while(isspace(source[temp_current])) temp_current++;

        int next_word_start = temp_current;
        while(isalnum(source[temp_current]) || source[temp_current] == '_') temp_current++;

        std::string first_word = source.substr(start, current - start);
        std::string second_word = source.substr(next_word_start, temp_current - next_word_start);
        std::string full_lexeme = first_word + " " + second_word;

        auto it = runtime_keyword_map.find(full_lexeme);
        if (it != runtime_keyword_map.end()) {
            current = temp_current;
            addToken(it->second, full_lexeme);
            return;
        }
    }

    std::string text = source.substr(start, current - start);
    auto it = runtime_keyword_map.find(text);
    addToken(it != runtime_keyword_map.end() ? it->second : TokenType::IDENTIFIER, text);
}

} // namespace CHTL
