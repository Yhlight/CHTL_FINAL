#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include <stdexcept>
#include <unordered_map>

namespace CHTLJS {

const std::unordered_map<std::string, CHTLJSTokenType> CHTLJSLexer::keywords = {
    {"listen", CHTLJSTokenType::Listen},
    {"animate", CHTLJSTokenType::Animate},
    {"delegate", CHTLJSTokenType::Delegate},
    {"router", CHTLJSTokenType::Router},
    {"vir", CHTLJSTokenType::Vir},
};

CHTLJSLexer::CHTLJSLexer(const std::string& source) : source_(source) {}

std::vector<CHTLJSToken> CHTLJSLexer::scanTokens() {
    while (!isAtEnd()) {
        start_ = current_;
        scanToken();
    }
    tokens_.push_back({CHTLJSTokenType::EndOfFile, "", line_, 0});
    return tokens_;
}

void CHTLJSLexer::scanToken() {
    char c = advance();
    switch (c) {
        case '{':
            if (peek() == '{') {
                advance();
                addToken(CHTLJSTokenType::OpenDoubleBrace);
            } else {
                addToken(CHTLJSTokenType::OpenBrace);
            }
            break;
        case '}':
            if (peek() == '}') {
                advance();
                addToken(CHTLJSTokenType::CloseDoubleBrace);
            } else {
                addToken(CHTLJSTokenType::CloseBrace);
            }
            break;
        case '[': addToken(CHTLJSTokenType::OpenBracket); break;
        case ']': addToken(CHTLJSTokenType::CloseBracket); break;
        case ':': addToken(CHTLJSTokenType::Colon); break;
        case ',': addToken(CHTLJSTokenType::Comma); break;
        case '=': addToken(CHTLJSTokenType::Equals); break;
        case '-':
             if (peek() == '>') {
                advance();
                addToken(CHTLJSTokenType::Arrow);
            }
            break;
        case '.':
            addToken(CHTLJSTokenType::Dot);
            break;
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            // Ignore whitespace
            break;
        case '\'':
        case '"':
            stringLiteral(c);
            break;
        default:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                identifier();
            } else if (c >= '0' && c <= '9') {
                number();
            } else if (c == '/' && peek() == '/') {
                while(peek() != '\n' && !isAtEnd()) advance();
            }
            break;
    }
}

void CHTLJSLexer::identifier() {
    while ((peek() >= 'a' && peek() <= 'z') || (peek() >= 'A' && peek() <= 'Z') || (peek() >= '0' && peek() <= '9') || peek() == '_') {
        advance();
    }
    std::string text = source_.substr(start_, current_ - start_);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(CHTLJSTokenType::Identifier);
    }
}

void CHTLJSLexer::number() {
    while (peek() >= '0' && peek() <= '9') advance();
    if (peek() == '.' && (peekNext() >= '0' && peekNext() <= '9')) {
        advance();
        while (peek() >= '0' && peek() <= '9') advance();
    }
    addToken(CHTLJSTokenType::Number);
}

void CHTLJSLexer::stringLiteral(char quote) {
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') line_++;
        advance();
    }
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string literal.");
    }
    advance(); // The closing quote
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    tokens_.push_back({CHTLJSTokenType::String, value, line_, 0});
}

char CHTLJSLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char CHTLJSLexer::peekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char CHTLJSLexer::advance() {
    current_++;
    return source_[current_ - 1];
}

bool CHTLJSLexer::isAtEnd() const {
    return current_ >= source_.length();
}

void CHTLJSLexer::addToken(CHTLJSTokenType type) {
    tokens_.push_back({type, source_.substr(start_, current_ - start_), line_, 0});
}

} // namespace CHTLJS
