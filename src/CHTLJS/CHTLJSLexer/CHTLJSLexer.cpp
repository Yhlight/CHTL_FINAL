#include "CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include <stdexcept>

namespace CHTLJS {

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
                advance(); // consume second '{'
                addToken(CHTLJSTokenType::OpenDoubleBrace);
                // Scan for the identifier inside
                start_ = current_;
                while (peek() != '}' && !isAtEnd()) {
                    advance();
                }
                if (current_ > start_) {
                    tokens_.push_back({CHTLJSTokenType::Identifier, source_.substr(start_, current_ - start_), line_, 0});
                }
            }
            break;
        case '}':
            if (peek() == '}') {
                advance(); // consume second '}'
                addToken(CHTLJSTokenType::CloseDoubleBrace);
            }
            break;
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
            // For now, we only care about the {{...}} syntax
            // In the future, this will handle other CHTL JS features
            break;
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
