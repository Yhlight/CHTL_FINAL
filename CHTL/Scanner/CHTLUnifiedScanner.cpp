#include "CHTLUnifiedScanner.h"
#include <stdexcept>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source) : source(source) {}

ScanResult CHTLUnifiedScanner::scan() {
    while (!isAtEnd()) {
        scanDefault();
    }
    return {sanitized_source_buffer, placeholders};
}

void CHTLUnifiedScanner::scanDefault() {
    skipWhitespace();
    if (isAtEnd()) return;

    // A simple check for top-level `style` or `script` keywords.
    // This is a naive implementation and doesn't use a proper CHTL lexer,
    // so it might misidentify keywords inside comments or strings.
    // This is a limitation we accept for this initial version.
    if (source.substr(current_pos, 5) == "style") {
        scanStyleBlock();
    } else if (source.substr(current_pos, 6) == "script") {
        scanScriptBlock();
    } else {
        sanitized_source_buffer += peek();
        advance();
    }
}

void CHTLUnifiedScanner::scanStyleBlock() {
    // Consume 'style' keyword
    for (int i = 0; i < 5; ++i) advance();
    skipWhitespace();

    std::string block_content = extractBlockContent();
    std::string placeholder = "__CHTL_CSS_PLACEHOLDER_" + std::to_string(style_placeholder_count++) + "__";

    sanitized_source_buffer += "style { " + placeholder + " }";
    placeholders[placeholder] = block_content;
}

void CHTLUnifiedScanner::scanScriptBlock() {
    // Consume 'script' keyword
    for (int i = 0; i < 6; ++i) advance();
    skipWhitespace();

    std::string block_content = extractBlockContent();
    std::string placeholder = "__CHTL_JS_PLACEHOLDER_" + std::to_string(script_placeholder_count++) + "__";

    sanitized_source_buffer += "script { " + placeholder + " }";
    placeholders[placeholder] = block_content;
}

std::string CHTLUnifiedScanner::extractBlockContent() {
    if (peek() != '{') {
        // For now, we assume style/script blocks are always enclosed in braces
        // as per CHTL.md's examples for global style/script.
        return "";
    }
    advance(); // Consume '{'

    std::string content;
    int brace_level = 1;

    while (!isAtEnd() && brace_level > 0) {
        if (peek() == '{') {
            brace_level++;
        } else if (peek() == '}') {
            brace_level--;
        }

        if (brace_level > 0) {
            content += peek();
        }
        advance();
    }

    if (brace_level != 0) {
        throw std::runtime_error("Mismatched braces in style or script block.");
    }

    return content;
}


// --- Private Helper Methods ---

void CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        current_pos++;
    }
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current_pos];
}

bool CHTLUnifiedScanner::isAtEnd() {
    return current_pos >= source.length();
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        sanitized_source_buffer += peek();
        advance();
    }
}

} // namespace CHTL
