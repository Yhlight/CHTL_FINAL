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
        case ':': addToken(CHTLJSTokenType::Colon); break;
        case ',': addToken(CHTLJSTokenType::Comma); break;
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
        default:
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
                identifier();
            } else if (c == '/' && peek() == '/') {
                while(peek() != '\n' && !isAtEnd()) advance();
            }
            else {
                // For now, assume the rest is a JS callback body
                // This is a major simplification. A real implementation would need to parse JS properly.
                 while (peek() != ',' && peek() != '}' && !isAtEnd()) {
                    advance();
                }
                addToken(CHTLJSTokenType::Identifier);
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

char CHTLJSLexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
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
