#include "Scanner/CHTLUnifiedScanner.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

// Constructor
CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source_(source) {}

// Public scan method: the entry point
std::vector<CodeChunk> CHTLUnifiedScanner::scan() {
    while (!isAtEnd()) {
        start_pos_ = current_pos_;
        scanNextToken();
    }
    return chunks_;
}

// scanNextToken: Identifies and dispatches to the correct handler
void CHTLUnifiedScanner::scanNextToken() {
    skipWhitespace();
    start_pos_ = current_pos_;

    if (isAtEnd()) return;

    // Check for block keywords. This logic will be expanded.
    if (match("script")) {
        handleScriptBlock();
    } else if (match("style")) {
        handleStyleBlock();
    } else if (peek() == '[') {
        handleBracketBlock();
    } else {
        handleGenericChtl();
    }
}

// --- Block Handlers ---

void CHTLUnifiedScanner::handleScriptBlock() {
    // Consume "script" keyword
    for (int i = 0; i < 6; ++i) advance();
    skipWhitespace();
    if (peek() != '{') return; // Should be an error
    advance(); // Consume '{'

    int brace_level = 1;
    start_pos_ = current_pos_;

    while (!isAtEnd() && brace_level > 0) {
        bool is_chtl_js_construct = match("{{") || match("listen") || match("delegate") || match("animate") || match("vir") || match("router") || (peek() == '$' && current_pos_ + 1 < source_.length() && isalnum(source_[current_pos_+1]));

        if (is_chtl_js_construct) {
            // Finalize the JavaScript chunk before the CHTL JS construct
            if (current_pos_ > start_pos_) {
                addChunk(ChunkType::JavaScript);
            }
            start_pos_ = current_pos_;

            // Consume and add the CHTL JS chunk
            if (match("{{")) {
                consumeChtlJsEnhancedSelector();
            } else if (peek() == '$') {
                consumeChtlJsResponsiveValue();
            } else {
                consumeChtlJsBlock();
            }
            addChunk(ChunkType::ChtlJs);
            start_pos_ = current_pos_;
        } else {
            if (peek() == '{') brace_level++;
            else if (peek() == '}') brace_level--;
            advance();
        }
    }

    // Add any remaining JS at the end of the block
    size_t end_pos = (current_pos_ > 0) ? current_pos_ - 1 : 0;
    if (start_pos_ < end_pos) {
        // To use addChunk, we set current_pos to the end of the chunk
        size_t temp_current = current_pos_;
        current_pos_ = end_pos;
        addChunk(ChunkType::JavaScript);
        current_pos_ = temp_current;
    }
    start_pos_ = current_pos_;
}

void CHTLUnifiedScanner::handleStyleBlock() {
    // Consume "style" keyword
    for (int i = 0; i < 5; ++i) advance();
    skipWhitespace();
    if (peek() != '{') return; // Error case
    advance(); // Consume '{'

    start_pos_ = current_pos_; // Mark start of CSS content
    int brace_level = 1;
    while (!isAtEnd() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;
        advance();
    }

    // Add the content inside the braces as a CSS chunk
    size_t end_pos = (current_pos_ > 0) ? current_pos_ - 1 : 0;
    if (start_pos_ < end_pos) {
        size_t temp_current = current_pos_;
        current_pos_ = end_pos;
        addChunk(ChunkType::Css);
        current_pos_ = temp_current;
    }
    start_pos_ = current_pos_;
}

void CHTLUnifiedScanner::handleBracketBlock() {
    // Consume until the closing ']'
    while (!isAtEnd() && peek() != ']') {
        advance();
    }
    if (peek() == ']') advance();
    skipWhitespace();

    // If it's a block construct, consume the block too.
    if (!isAtEnd() && peek() == '{') {
        advance(); // consume '{'
        int brace_level = 1;
        while(!isAtEnd() && brace_level > 0) {
            if (peek() == '{') brace_level++;
            else if (peek() == '}') brace_level--;
            advance();
        }
    }
    // The whole thing is one CHTL chunk
    addChunk(ChunkType::CHTL);
}

void CHTLUnifiedScanner::handleGenericChtl() {
    int brace_level = 0;
    while (!isAtEnd()) {
        // Stop if we find a top-level block keyword
        if (brace_level == 0 && (match("script") || match("style") || peek() == '[')) {
            break;
        }

        if (peek() == '{') {
            brace_level++;
        } else if (peek() == '}') {
            if (brace_level > 0) {
                brace_level--;
            }
        }
        advance();
    }
    addChunk(ChunkType::CHTL);
}

// --- CHTL JS Specific Parsers ---

void CHTLUnifiedScanner::consumeChtlJsBlock() {
    // Consume keyword
    while (!isAtEnd() && isalpha(peek())) {
        advance();
    }
    skipWhitespace();
    if (isAtEnd() || peek() != '{') return;
    advance(); // consume '{'

    int brace_level = 1;
    while (!isAtEnd() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;
        advance();
    }
}

void CHTLUnifiedScanner::consumeChtlJsEnhancedSelector() {
    advance(); // consume '{'
    advance(); // consume '{'
    while (!isAtEnd() && !match("}}")) {
        advance();
    }
    if (match("}}")) {
        advance(); // consume '}'
        advance(); // consume '}'
    }
}

void CHTLUnifiedScanner::consumeChtlJsResponsiveValue() {
    advance(); // consume '$'
    while (!isAtEnd() && isalnum(peek())) {
        advance();
    }
    if (peek() == '$') {
        advance(); // consume closing '$'
    }
}


// --- Utility Methods ---

bool CHTLUnifiedScanner::isAtEnd() const {
    return current_pos_ >= source_.length();
}

char CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        current_pos_++;
    }
    return source_[current_pos_ - 1];
}

char CHTLUnifiedScanner::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_pos_];
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (current_pos_ + expected.length() > source_.length()) {
        return false;
    }
    // For {{ and }}, we don't need a word boundary check
    if (expected == "{{" || expected == "}}") {
         return source_.substr(current_pos_, expected.length()) == expected;
    }

    if (source_.substr(current_pos_, expected.length()) == expected) {
        // Ensure it's a whole word, followed by a non-alphanumeric character
        size_t next_char_pos = current_pos_ + expected.length();
        if (next_char_pos == source_.length() || !std::isalnum(source_[next_char_pos])) {
            return true;
        }
    }
    return false;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

void CHTLUnifiedScanner::addChunk(ChunkType type) {
    if (start_pos_ < current_pos_) {
        std::string content = source_.substr(start_pos_, current_pos_ - start_pos_);
        if (!content.empty()) {
            chunks_.push_back({type, content});
        }
    }
}

} // namespace CHTL
