#include "CHTLUnifiedScanner.h"
#include <iostream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source)
    : source(source), current(0), line(1) {}

bool CHTLUnifiedScanner::isAtEnd() {
    return current >= source.length();
}

char CHTLUnifiedScanner::advance() {
    if (!isAtEnd()) {
        if (source[current] == '\n') {
            line++;
        }
        current++;
    }
    return source[current - 1];
}

char CHTLUnifiedScanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void CHTLUnifiedScanner::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance();
        } else if (c == '/' && (current + 1 < source.length() && source[current + 1] == '/')) {
            while (peek() != '\n' && !isAtEnd()) advance();
        } else if (c == '/' && (current + 1 < source.length() && source[current + 1] == '*')) {
            advance(); advance(); // Consume /*
            while (!isAtEnd() && !(peek() == '*' && (current + 1 < source.length() && source[current + 1] == '/'))) {
                advance();
            }
            if (!isAtEnd()) { advance(); advance(); } // Consume */
        } else {
            return;
        }
    }
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan() {
    std::vector<CodeFragment> fragments;
    size_t last_fragment_end = 0;
    int brace_level = 0;

    for (size_t i = 0; i < source.length(); ++i) {
        if (source[i] == '{') {
            brace_level++;
        } else if (source[i] == '}') {
            if (brace_level > 0) {
                brace_level--;
            }
        } else if (brace_level == 0) {
            // Only check for keywords at the top level
            bool is_style = (source.substr(i, 5) == "style" && !isalnum(source[i+5]));
            bool is_script = (source.substr(i, 6) == "script" && !isalnum(source[i+6]));

            if (is_style || is_script) {
                size_t keyword_end = i + (is_style ? 5 : 6);
                size_t block_start = source.find('{', keyword_end);

                if (block_start != std::string::npos) {
                    // Check if there is anything other than whitespace between keyword and brace
                    bool valid_block = true;
                    for(size_t j = keyword_end; j < block_start; ++j) {
                        if (!isspace(source[j])) {
                            valid_block = false;
                            break;
                        }
                    }

                    if (valid_block) {
                        // Found a top-level block.
                        // 1. Add preceding CHTL fragment
                        if (i > last_fragment_end) {
                            fragments.push_back({FragmentType::CHTL, source.substr(last_fragment_end, i - last_fragment_end), 0});
                        }

                        // 2. Find matching brace and add block fragment
                        int inner_brace_level = 1;
                        size_t block_content_start = block_start + 1;
                        size_t block_end = block_content_start;
                        while (block_end < source.length() && inner_brace_level > 0) {
                            if (source[block_end] == '{') inner_brace_level++;
                            else if (source[block_end] == '}') inner_brace_level--;
                            block_end++;
                        }

                        if (inner_brace_level == 0) {
                             std::string content = source.substr(block_content_start, block_end - block_content_start - 1);
                             fragments.push_back({is_style ? FragmentType::CSS : FragmentType::JS, content, 0});
                             last_fragment_end = block_end;
                             i = block_end -1; // continue scan after the block
                        }
                    }
                }
            }
        }
    }

    // Add the final remaining CHTL fragment
    if (last_fragment_end < source.length()) {
        fragments.push_back({FragmentType::CHTL, source.substr(last_fragment_end), 0});
    }

    return fragments;
}

} // namespace CHTL
