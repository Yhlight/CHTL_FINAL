#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    size_t last_pos = 0;
    current = 0;
    line = 1;
    fragments.clear();

    while (current < source.length()) {
        size_t next_style = source.find("style", current);
        size_t next_script = source.find("script", current);

        size_t next_pos = std::string::npos;
        bool is_style = false;

        if (next_style != std::string::npos && (next_script == std::string::npos || next_style < next_script)) {
            next_pos = next_style;
            is_style = true;
        } else if (next_script != std::string::npos) {
            next_pos = next_script;
            is_style = false;
        }

        if (next_pos == std::string::npos) {
            buffer = source.substr(last_pos);
            finalizeCurrentFragment(FragmentType::CHTL);
            break;
        }

        size_t keyword_len = is_style ? 5 : 6;
        size_t temp_current = next_pos + keyword_len;
        while(temp_current < source.length() && isspace(source[temp_current])) {
            temp_current++;
        }

        if (temp_current < source.length() && source[temp_current] == '{') {
            buffer = source.substr(last_pos, next_pos - last_pos);
            finalizeCurrentFragment(FragmentType::CHTL);

            current = next_pos;
            if (is_style) {
                handleStyleBlock();
            } else {
                handleScriptBlock();
            }
            last_pos = current;
        } else {
            current = next_pos + keyword_len;
        }
    }

    return fragments;
}

void CHTLUnifiedScanner::finalizeCurrentFragment(FragmentType type) {
    if (!buffer.empty()) {
        size_t start = buffer.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            buffer.clear();
            return;
        }
        buffer = buffer.substr(start);
        size_t end = buffer.find_last_not_of(" \t\n\r");
        buffer = buffer.substr(0, end + 1);

        if (!buffer.empty()) {
            fragments.push_back({type, buffer, line_start});
        }
    }
    buffer.clear();
}

void CHTLUnifiedScanner::handleStyleBlock() {
    current += 5;
    skipWhitespace();

    advance();
    int brace_level = 1;
    size_t content_start = current;

    while (brace_level > 0 && !isAtEnd()) {
        if (peek() == '"' || peek() == '\'') skipStringLiteral();
        else if (peek() == '/' && peekNext() == '/') skipLineComment();
        else if (peek() == '/' && peekNext() == '*') skipBlockComment();
        else if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;

        if (brace_level > 0) advance();
    }

    size_t content_end = current;
    buffer = source.substr(content_start, content_end - content_start);
    finalizeCurrentFragment(FragmentType::CSS);
    if (!isAtEnd()) advance();
}

void CHTLUnifiedScanner::handleScriptBlock() {
    current += 6;
    skipWhitespace();

    advance();
    int brace_level = 1;
    size_t block_start = current;

    while (brace_level > 0 && !isAtEnd()) {
        if (peek() == '"' || peek() == '\'') skipStringLiteral();
        else if (peek() == '/' && peekNext() == '/') skipLineComment();
        else if (peek() == '/' && peekNext() == '*') skipBlockComment();
        else if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;
        if (brace_level > 0) advance();
    }
    size_t block_end = current;

    std::string script_content = source.substr(block_start, block_end - block_start);
    scanJsAndChtlJs(script_content);

    if (!isAtEnd()) advance();
}

// Final logic for JS/CHTL JS separation
void CHTLUnifiedScanner::scanJsAndChtlJs(const std::string& script_source) {
    std::string final_chtl_js_buffer;
    std::string current_js_buffer;
    size_t script_current = 0;

    auto finalize_current_js_chunk = [&]() {
        if (!current_js_buffer.empty()) {
            fragments.push_back({FragmentType::JS, current_js_buffer, line});
            final_chtl_js_buffer += JS_PLACEHOLDER;
            current_js_buffer.clear();
        }
    };

    while (script_current < script_source.length()) {
        // Check for CHTL JS tokens
        if (script_source.substr(script_current, 2) == "{{") {
            finalize_current_js_chunk();
            size_t start = script_current;
            script_current += 2; // Move past '{{'
            while(script_current < script_source.length() && script_source.substr(script_current, 2) != "}}") {
                script_current++;
            }
            if(script_current < script_source.length()) script_current += 2; // Move past '}}'
            final_chtl_js_buffer += script_source.substr(start, script_current - start);
            continue;
        }

        if (script_source.substr(script_current, 2) == "->") {
             finalize_current_js_chunk();
             final_chtl_js_buffer += "->";
             script_current += 2;
             continue;
        }

        bool keyword_found = false;
        for (const auto& keyword : CHTL_JS_KEYWORDS) {
            if (script_source.substr(script_current, keyword.length()) == keyword &&
                (script_current + keyword.length() >= script_source.length() || !isalnum(script_source[script_current + keyword.length()])))
            {
                 finalize_current_js_chunk();
                 final_chtl_js_buffer += keyword;
                 script_current += keyword.length();
                 keyword_found = true;
                 break;
            }
        }
        if (keyword_found) continue;

        // Not a CHTL JS token, so it's part of a JS chunk AND the CHTL JS structure
        char c = script_source[script_current];
        current_js_buffer += c;
        final_chtl_js_buffer += c;
        script_current++;
    }

    finalize_current_js_chunk();

    if (!final_chtl_js_buffer.empty()) {
        fragments.push_back({FragmentType::CHTL_JS, final_chtl_js_buffer, line});
    }
}


bool CHTLUnifiedScanner::isAtEnd() const {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (current < source.length()) {
        if (source[current] == '\n') line++;
        current++;
        return source[current-1];
    }
    return '\0';
}

char CHTLUnifiedScanner::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLUnifiedScanner::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (current + expected.length() > source.length()) return false;
    return source.substr(current, expected.length()) == expected;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd() && isspace(peek())) {
        advance();
    }
}

void CHTLUnifiedScanner::skipLineComment() {
    if (peek() == '/' && peekNext() == '/') {
        while (peek() != '\n' && !isAtEnd()) advance();
    }
}

void CHTLUnifiedScanner::skipBlockComment() {
    if (peek() == '/' && peekNext() == '*') {
        advance(); advance();
        while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) advance();
        if (!isAtEnd()) { advance(); advance(); }
    }
}

void CHTLUnifiedScanner::skipStringLiteral() {
    char quote = advance();
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\\') advance();
        advance();
    }
    if (!isAtEnd()) advance();
}

bool CHTLUnifiedScanner::isChtlJsKeyword() {
    return false;
}

} // namespace CHTL
