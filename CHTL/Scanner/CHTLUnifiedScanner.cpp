#include "CHTLUnifiedScanner.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source) {}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    if (fragments.empty()) {
        scanSource();
    }
    return fragments;
}

const std::map<std::string, std::string>& CHTLUnifiedScanner::getPlaceholders() const {
    return placeholders;
}

void CHTLUnifiedScanner::scanSource() {
    size_t last_fragment_end = 0;

    while (current < source.length()) {
        size_t style_pos = source.find("style", current);
        size_t script_pos = source.find("script", current);

        size_t next_pos = std::string::npos;
        bool is_style = false;

        if (style_pos != std::string::npos && (script_pos == std::string::npos || style_pos < script_pos)) {
            next_pos = style_pos;
            is_style = true;
        } else if (script_pos != std::string::npos) {
            next_pos = script_pos;
            is_style = false;
        } else {
            break;
        }

        std::string keyword = is_style ? "style" : "script";
        bool is_word_boundary_start = (next_pos == 0) || !isalnum(source[next_pos - 1]);
        bool is_word_boundary_end = (next_pos + keyword.length() >= source.length()) || !isalnum(source[next_pos + keyword.length()]);

        if (!is_word_boundary_start || !is_word_boundary_end) {
             current = next_pos + 1;
             continue;
        }

        size_t temp_current = current;
        current = next_pos + keyword.length();
        skipWhitespace();

        if (isAtEnd() || peek() != '{') {
            current = next_pos + 1;
            continue;
        }

        if (next_pos > last_fragment_end) {
            fragments.push_back({FragmentType::CHTL, source.substr(last_fragment_end, next_pos - last_fragment_end), line});
        }

        current = next_pos;
        if (is_style) {
            scanStyleBlock();
        } else {
            scanScriptBlock();
        }

        last_fragment_end = current;
    }

    if (last_fragment_end < source.length()) {
        fragments.push_back({FragmentType::CHTL, source.substr(last_fragment_end), line});
    }
}


void CHTLUnifiedScanner::scanStyleBlock() {
    match("style");
    skipWhitespace();
    if (isAtEnd() || peek() != '{') return;
    advance();

    size_t start_pos = current;
    int brace_level = 1;
    while (!isAtEnd() && brace_level > 0) {
        if (peek() == '{') brace_level++;
        else if (peek() == '}') brace_level--;

        if (brace_level > 0) advance();
    }

    std::string content = source.substr(start_pos, current - start_pos);
    fragments.push_back({FragmentType::CSS, content, line});

    if (!isAtEnd() && peek() == '}') advance();
}

void CHTLUnifiedScanner::scanScriptBlock() {
    match("script");
    skipWhitespace();
    if (isAtEnd() || peek() != '{') return;
    advance();

    size_t last_pos = current;
    int brace_level = 1;

    while (!isAtEnd() && brace_level > 0) {
        if (isChtlJsEnhanceSelector() || isChtlJsKeyword()) {
            if (current > last_pos) {
                std::string js_content = source.substr(last_pos, current - last_pos);
                generatePlaceholder(js_content);
            }

            size_t chtl_js_start = current;
            while(true) {
                size_t backtrack_pos = current;
                skipWhitespace();

                if (isChtlJsEnhanceSelector()) {
                    advance(); advance(); // Consume {{
                    while (!isAtEnd() && !(peek() == '}' && peekNext() == '}')) advance();
                    if (!isAtEnd()) advance(); // }
                    if (!isAtEnd()) advance(); // }

                    skipWhitespace();
                    if (!isAtEnd() && peek() == '-' && peekNext() == '>') {
                        advance(); advance(); // Consume ->
                    } else if (!isAtEnd() && peek() == '&' && peekNext() == '-') {
                         if (current + 2 < source.length() && source[current+2] == '>') {
                             advance(); advance(); advance(); // Consume &->
                         }
                    }
                } else if (isChtlJsKeyword()) {
                    while (!isAtEnd() && isalnum(peek())) advance();
                    skipWhitespace();
                    if (!isAtEnd() && peek() == '{') {
                        advance();
                        int keyword_brace_level = 1;
                        while (!isAtEnd() && keyword_brace_level > 0) {
                            if (peek() == '{') keyword_brace_level++;
                            else if (peek() == '}') keyword_brace_level--;
                            if (keyword_brace_level > 0) advance();
                        }
                        if (!isAtEnd()) advance();
                    }
                } else {
                    current = backtrack_pos; // Backtrack if no more CHTL JS found
                    break;
                }
            }

            std::string chtl_js_content = source.substr(chtl_js_start, current - chtl_js_start);
            if (!chtl_js_content.empty()) {
                fragments.push_back({FragmentType::CHTL_JS, chtl_js_content, line});
            }

            last_pos = current;

        } else {
            if (peek() == '{') brace_level++;
            else if (peek() == '}') brace_level--;

            if (brace_level > 0) {
                advance();
            }
        }
    }

    if (current > last_pos) {
        std::string js_content = source.substr(last_pos, current - last_pos);
        generatePlaceholder(js_content);
    }

    if (!isAtEnd() && peek() == '}') {
        advance();
    }
}


char CHTLUnifiedScanner::advance() {
    if (isAtEnd()) return '\0';
    if (source[current] == '\n') line++;
    current++;
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char CHTLUnifiedScanner::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool CHTLUnifiedScanner::isAtEnd() const {
    return current >= source.length();
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    advance(); advance();
                    while(!isAtEnd() && (peek() != '*' || peekNext() != '/')) {
                        advance();
                    }
                    if(!isAtEnd()) advance();
                    if(!isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

bool CHTLUnifiedScanner::match(const std::string& expected) {
    if (source.rfind(expected, current) == current) {
       current += expected.length();
        return true;
    }
    return false;
}

std::string CHTLUnifiedScanner::generatePlaceholder(const std::string& content) {
    if (content.find_first_not_of(" \t\n\r") == std::string::npos) {
        return "";
    }
    std::string placeholder_id = "_JS_CODE_PLACEHOLDER_" + std::to_string(placeholder_counter++);
    placeholders[placeholder_id] = content;
    fragments.push_back({FragmentType::JS_PLACEHOLDER, placeholder_id, line});
    return placeholder_id;
}

bool CHTLUnifiedScanner::isChtlJsKeyword() {
    const static std::vector<std::string> keywords = {"Listen", "Animate", "Router", "Delegate", "ScriptLoader", "printMylove", "iNeverAway", "util", "Vir"};
    for (const auto& keyword : keywords) {
        if (source.rfind(keyword, current) == current) {
             size_t end_of_keyword = current + keyword.length();
             if (end_of_keyword >= source.length() || !isalnum(source[end_of_keyword])) {
                 return true;
             }
        }
    }
    return false;
}

bool CHTLUnifiedScanner::isChtlJsEnhanceSelector() {
    if (peek() == '{' && peekNext() == '{') {
        return true;
    }
    return false;
}

void CHTLUnifiedScanner::scanChtlJs() {
}

void CHTLUnifiedScanner::scanJs() {
}

}
